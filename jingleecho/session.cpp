
#include <iostream>
#include <sstream>
#include <cstdlib>

#include <gst/gstpad.h>
#include "conference.h"

#include "logger/logger.h"
#include "fstjingle.h"

/** @brief Create one media stream session.
    @param conf Conference class, to which it will belong.
    @param type Type of media transmitted with this session.
*/
Session::Session(Conference *conf, FsMediaType type)
    : m_conf(conf),m_session(NULL),m_lasterror(NULL),m_stream(NULL),
      m_localCandidates(NULL)
{
    GError *err =NULL;
    m_session = fs_conference_new_session(FS_CONFERENCE(conf->conference()), type, &err);
    if (err != NULL) {
        LOGGER(logit) << " fs_conference_new_session: "<< err->message << std::endl;  
        conf->reportFatalError(err->message);
        g_error_free(err);
    }
    g_assert(m_session);
    g_signal_connect(m_session, "error", G_CALLBACK(Session::sessionError), this); 
}

Session::~Session()
{
    g_object_unref(m_stream);
    g_object_unref(m_session);
    fs_candidate_list_destroy(m_localCandidates);
}

FsStream *Session::createStream(FsParticipant *participant, const GList *lcandidates)
{
    GParameter param[4];
    int paramcount = 0;

    memset(&param, 0, sizeof(param));

    if (lcandidates) {
        param[paramcount].name = "preferred-local-candidates";
        g_value_init(&param[paramcount].value, FS_TYPE_CANDIDATE_LIST);
        g_value_take_boxed(&param[paramcount].value, lcandidates);
        paramcount++;
    }
    if (!m_conf->stunIp().empty()) {
        param[paramcount].name = "stun-ip";
        g_value_init(&param[paramcount].value, G_TYPE_STRING);
        g_value_set_string(&param[paramcount].value, m_conf->stunIp().c_str());
        paramcount++;
    }
    if (m_conf->stunPort() > 0) {
        param[paramcount].name = "stun-port";
        g_value_init(&param[paramcount].value, G_TYPE_UINT);
        g_value_set_uint(&param[paramcount].value, m_conf->stunPort());
        paramcount++;
    }
    FsStream *stream = fs_session_new_stream(
        m_session,
        participant,
        FS_DIRECTION_BOTH,
        m_conf->transmitter().c_str(),
        paramcount, param, &m_lasterror);
    if (m_lasterror) {
        LOGGER(logit) << " fs_session_new_stream: " 
                  << m_lasterror->message << std::endl;
    }
    return stream;
}

bool Session::createStream(FsParticipant *participant)
{
    m_stream = createStream(participant, m_localCandidates);
    if (!m_stream) 
        return false;
    g_signal_connect(m_stream, "src-pad-added", G_CALLBACK(Session::srcPadAdded), this);
    g_signal_connect(m_stream, "error", G_CALLBACK(Session::streamError), this);
    return (m_stream != NULL);
}

/** @brief Create farsight stream inside this structure, fill with remote participant
 *  from local conference, and local candidates set before calling this function. */
bool Session::createStream()
{
    return createStream(m_conf->remoteParticipant());
}
   
/** @brief Check if stream is created. 
    @return true if stream is already created, false otherwise. */ 
bool Session::haveStream()
{
    return (m_stream != NULL);
}

void Session::setRemote(const std::string &ip, int port)
{
    const gchar *foundation = "foundation-test";

    FsCandidate * candidate = fs_candidate_new(foundation, 1, 
        FS_CANDIDATE_TYPE_HOST, FS_NETWORK_PROTOCOL_UDP,
        ip.c_str(), port);

    GList *candidates = g_list_prepend(NULL, candidate);

    g_assert(m_stream);

    fs_stream_set_remote_candidates(m_stream, candidates, &m_lasterror);

    fs_candidate_list_destroy(candidates);
}

/** @brief set remote candidates from GList of (FsCandidate *) */
void Session::setRemote(GList *list)
{
    g_assert(m_session);
    fs_stream_set_remote_candidates(m_stream, list, &m_lasterror);
    if (m_lasterror) {
        LOGGER(logit) << "fs_stream_set_remote_candidates: " 
            << m_lasterror->message << std::endl;
        resetError();    
    }
}

void Session::setLocalCodec(GList *codecs)
{
    g_assert(m_session);
    fs_session_set_codec_preferences(m_session, codecs, &m_lasterror);
    if (m_lasterror) {
        LOGGER(logit) << "fs_stream_set_codec_preferences" 
            << m_lasterror->message << std::endl;
        resetError();
    }
}

bool Session::setRemoteCodec(GList *codecs)
{
    if (m_stream) {
        bool r=  fs_stream_set_remote_codecs(m_stream, codecs, &m_lasterror);
        if (m_lasterror) {
            LOGGER(logit) << "fs_stream_set_remote_codecs: "
                << m_lasterror->message << std::endl;
            resetError();
        }
        return r;
    }
    return false;
}

/** @brief Returns sink pad, where should be sended data sent to.
    You have to unref this pad when not needed. */
GstPad * Session::sink()
{
    GstPad *sink = NULL;
    g_object_get(G_OBJECT(m_session), "sink-pad", &sink, NULL);
    return sink;
}

/** @brief Set local candidates for this stream. 
    @param candidates GList of FsCandidate. There might be NULL candidate->ip. */
void Session::setLocal(GList *candidates)
{
    if (m_localCandidates)
        fs_candidate_list_destroy(m_localCandidates);
    m_localCandidates = fs_candidate_list_copy(candidates);
}

void Session::srcPadAdded(FsStream *stream, GstPad *pad, FsCodec *codec, gpointer user_data)
{
    Session *session = (Session *) user_data;
    LOGGER(logit) << "Source pad added" << std::endl;
    session->m_conf->srcPadAdded(session, pad, codec);
}

void Session::streamError(FsStream *self, FsError errno, gchar *error_msg,
    gchar *debug_msg, gpointer user_data)
{
    Session *session = (Session *) user_data;
    LOGGER(logit).printf("Error on stream %p(%p): %d, %s, %s",
        self, session, errno, error_msg, debug_msg);
}

void Session::sessionError(FsSession *self, FsError errno, gchar *error_msg,
    gchar *debug_msg, gpointer user_data)
{
    Session *session = (Session *) user_data;
    LOGGER(logit).printf("Error on session %p(%p): %d, %s, %s",
        self, session, errno, error_msg, debug_msg);
}

FsCodec *Session::currentSendCodec()
{
    FsCodec *codec = NULL;
    g_object_get(G_OBJECT(m_session), "current-send-codec", &codec, NULL);
    return codec;
}

GList * Session::currentRecvCodec()
{
    GList *codec = NULL;
    if (m_stream)
        g_object_get(G_OBJECT(m_stream), "current-recv-codecs", &codec, NULL);
    return codec;
}

/** @brief Return state of codecs.
 * property "codecs-ready" of m_session. */
bool Session::codecsReady()
{
    gboolean ready = false;
    g_object_get(G_OBJECT(m_session), "codecs-ready", &ready, NULL);
    return ready;
}

unsigned int Session::rtpSsrc()
{
    guint ssrc;
    g_object_get(G_OBJECT(m_session), "ssrc", &ssrc, NULL);
    return ssrc;
}

int Session::rtcpTimeout()
{
    int timeout;
    g_object_get(G_OBJECT(m_session), "no-rtcp-timeout", &timeout, NULL);
    return timeout;
}

GList *Session::getCodecListProperty(const char *name)
{
    GList *list = NULL;
    g_object_get(G_OBJECT(m_session), name, &list, NULL);
    return list;
}

void Session::setRtcpTimeout(int timeout)
{
    g_object_set(G_OBJECT(m_session), "no-rtcp-timeout", timeout, NULL);
}

std::string Session::describe()
{
    std::ostringstream o("RTP Session ");
    o << " name: " << m_name;
    o << " codecs ready: " << codecsReady();
    o << " ssrc: " << rtpSsrc();
    o << " rtcp timeout: " << rtcpTimeout();
    if (m_stream) 
        o << " has stream";
    o << std::endl;

    GstPad *s = sink();
    if (s) {
        gchar *name = gst_pad_get_name(s);
        o << "sink pad: " << name;
        o << " active: " << gst_pad_is_active(s);
        g_free(name);
    }
    FsCodec * codec = currentSendCodec();
    if (codec)
        o<< " send codec: " << FstJingle::toString(codec);
    o << " codec preferences: " << FstJingle::codecListToString(getCodecListProperty("codec-preferences")) << std::endl;
    o << " codecs: " << FstJingle::codecListToString(getCodecListProperty("codecs"));
    return o.str(); 
}

void Session::resetError()
{
    if (m_lasterror) {
        g_error_free(m_lasterror);
    }
    m_lasterror = NULL;
}

std::string Session::name()
{
    return m_name;
}

void Session::setName(const std::string &name)
{
    m_name = name;
}

bool Session::isSrcLinked()
{
    return m_srclinked;
}

void Session::setSrcLinked(bool linked)
{
    m_srclinked = linked;
}

GList * Session::getLocalCandidates()
{
    return fs_candidate_list_copy(m_localCandidates);
}

void Session::clearLocalCandidates()
{
    if (m_localCandidates)
        fs_candidate_list_destroy(m_localCandidates);
    m_localCandidates = NULL;
}

void Session::onNewLocalCandidate(FsCandidate *candidate)
{
    FsCandidate *copy = fs_candidate_copy(candidate);
    m_localCandidates = g_list_prepend(m_localCandidates, copy);
}

FsSession   *Session::sessionElement()
{
    gst_object_ref(m_session);
    return m_session;
}

FsStream    *Session::streamElement()
{
    gst_object_ref(m_stream);
    return m_stream;
}

unsigned int Session::id()
{
    unsigned int id = -1;
    g_object_get(G_OBJECT(m_session), "id", &id, NULL);
    return id;
}

/** @brief Get FsStream::id property from passed stream. */    
unsigned int Session::idFromStream(FsStream *stream)
{
    FsSession *fs_session = NULL;
    unsigned int id = -1;
    g_object_get(G_OBJECT(stream), 
        "session", &fs_session, 
        NULL);
    if (fs_session)
        g_object_get(G_OBJECT(fs_session), "id", &id, NULL);
    return id;
}

/** @brief Get FsStream::id property from passed stream. */    
unsigned int Session::idFromStream(const GValue *val)
{
    g_assert(val);
    FsStream *fs_stream = (FsStream *) g_value_get_pointer(val);
    return idFromStream(fs_stream);
}

FsMediaType Session::type() const
{
    FsMediaType t = FS_MEDIA_TYPE_AUDIO;
    g_object_get(G_OBJECT(m_session), "media-type", &t, NULL);
    return t;
}

