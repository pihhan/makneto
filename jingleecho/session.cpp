
#include <iostream>
#include <sstream>

#include <gst/gstpad.h>
#include "conference.h"

#include "logger/logger.h"
#include "fsjingle.h"

static const gchar     stun_ip[] = "212.71.150.10";

Session::Session(Conference *conf)
    : m_conf(conf),m_session(NULL),m_lasterror(NULL),m_stream(NULL),
      m_localCandidates(NULL)
{
    GError *err =NULL;
    m_session = fs_conference_new_session(FS_CONFERENCE(conf->conference()), FS_MEDIA_TYPE_AUDIO, &err);
    if (err != NULL) {
        LOGGER(logit) << " fs_conference_new_session: "<< err->message << std::endl;  
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
    GParameter param[3] = { {0},};
    int paramcount = 0;

    if (lcandidates) {
        param[paramcount].name = "preferred-local-candidates";
        g_value_init(&param[paramcount].value, FS_TYPE_CANDIDATE_LIST);
        g_value_take_boxed(&param[paramcount].value, lcandidates);
        paramcount++;
    }
    if (*stun_ip) {
        param[paramcount].name = "stun-ip";
        g_value_init(&param[paramcount].value, G_TYPE_STRING);
        g_value_set_static_string(&param[paramcount].value, stun_ip);
        paramcount++;
    }
    FsStream *stream = fs_session_new_stream(
        m_session,
        participant,
        FS_DIRECTION_BOTH,
        "rawudp",
        paramcount, param, &m_lasterror);
    if (m_lasterror) {
        LOGGER(logit) << " fs_session_new_stream:" 
                  << m_lasterror->message << std::endl;
    }
    g_assert(stream);
    return stream;
}

/** @brief Create farsight stream inside this structure, fill with remote participant
 *  from local conference, and local candidates set before calling this function. */
bool Session::createStream()
{
    m_stream = createStream(m_conf->remoteParticipant(), m_localCandidates);
    g_assert(m_stream);
    g_signal_connect(m_stream, "src-pad-added", G_CALLBACK(Session::srcPadAdded), this);
    g_signal_connect(m_stream, "error", G_CALLBACK(Session::streamError), this);
    return true;
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

