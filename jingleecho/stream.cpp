
#include <iostream>
#include <sstream>
#include <cstdlib>

#include <gst/gstpad.h>
#include "conference.h"

#include "logger/logger.h"
#include "fstjingle.h"

#include "stream.h"

/** @brief Create one media stream session.
    @param conf Conference class, to which it will belong.
    @param type Type of media transmitted with this session.
*/
Stream(Session *session, FsParticipant *participant, const GList *lcandidates = NULL)
    : m_confsession(session)
{
    m_session = session->streamElement();
    m_stream = createStream(participant, lcandidates);
    g_signal_connect(m_stream, "src-pad-added", G_CALLBACK(Stream::srcPadAdded), this);
    g_signal_connect(m_stream, "error", G_CALLBACK(Stream::streamError), this);
}

Session::Session(Conference *conf, FsMediaType type)
    : m_conf(conf),m_session(NULL),m_lasterror(NULL),m_stream(NULL),
      m_localCandidates(NULL), m_newLocalCandidates(NULL)
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

Stream::~Stream()
{
    fs_candidate_list_destroy(m_localCandidates);
    g_object_unref(m_stream);
    g_object_unref(m_session);
}

Session *Stream::session()
{
    return m_confsession;
}

FsStream *Stream::createStream(FsParticipant *participant, const GList *lcandidates)
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

/** @brief Configure remote candidate. */
void Stream::setRemote(const std::string &ip, int port)
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
void Stream::setRemote(GList *list)
{
    g_assert(m_session);
    fs_stream_set_remote_candidates(m_stream, list, &m_lasterror);
    if (m_lasterror) {
        LOGGER(logit) << "fs_stream_set_remote_candidates: " 
            << m_lasterror->message << std::endl;
        resetError();    
    }
}

/** @brief Configure remote codecs. */
bool Stream::setRemoteCodec(GList *codecs)
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

/** @brief Set local candidates for this stream. 
    @param candidates GList of FsCandidate. There might be NULL candidate->ip. */
void Stream::setLocal(GList *candidates)
{
    if (m_localCandidates)
        fs_candidate_list_destroy(m_localCandidates);
    m_localCandidates = fs_candidate_list_copy(candidates);
}

/** @brief Configure local port, any address on host will be used. */
void Stream::setLocalPort(unsigned int port)
{
    if (m_localCandidates)
        fs_candidate_list_destroy(m_localCandidates);
    FsCandidate * candidate = fs_candidate_new(foundation, 1, 
        FS_CANDIDATE_TYPE_HOST, FS_NETWORK_PROTOCOL_UDP,
        NULL, port);
    m_localCandidates = g_list_prepend(NULL, candidate);
}

/** @brief React on incoming source data pad.
    Connect new pad to sink at conference. 
    This is called from gstreamer thread! */
void Stream::srcPadAdded(FsStream *stream, GstPad *pad, FsCodec *codec, gpointer user_data)
{
    Stream *s = (Stream *) user_data;
    LOGGER(logit) << "Source pad added" << std::endl;
    s->session()->m_conf->srcPadAdded(session, pad, codec);
}

void Stream::streamError(FsStream *self, FsError errno, gchar *error_msg,
    gchar *debug_msg, gpointer user_data)
{
    Session *session = (Session *) user_data;
    LOGGER(logit).printf("Error on stream %p(%p): %d, %s, %s",
        self, session, errno, error_msg, debug_msg);
}

/** @brief Return list of codecs of data we received. */
GList * Stream::currentRecvCodec()
{
    GList *codec = NULL;
    if (m_stream)
        g_object_get(G_OBJECT(m_stream), "current-recv-codecs", &codec, NULL);
    return codec;
}

GList *Stream::getCodecListProperty(const char *name)
{
    GList *list = NULL;
    g_object_get(G_OBJECT(m_stream), name, &list, NULL);
    return list;
}

std::string Stream::describe()
{
    std::ostringstream o("RTP stream ");
    o << " name: " << m_name;
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

void Stream::resetError()
{
    if (m_lasterror) {
        g_error_free(m_lasterror);
    }
    m_lasterror = NULL;
}

bool Session::isSrcLinked()
{
    return m_srclinked;
}

void Session::setSrcLinked(bool linked)
{
    m_srclinked = linked;
}

GList * Stream::getLocalCandidates()
{
    return fs_candidate_list_copy(m_localCandidates);
}

GList * Stream::getNewLocalCandidates()
{
    return fs_candidate_list_copy(m_newLocalCandidates);
}

void Stream::clearLocalCandidates()
{
    if (m_localCandidates)
        fs_candidate_list_destroy(m_localCandidates);
    m_localCandidates = NULL;
}

void Stream::onNewLocalCandidate(FsCandidate *candidate)
{
    FsCandidate *copy = fs_candidate_copy(candidate);
    m_newLocalCandidates = g_list_prepend(m_newLocalCandidates, copy);
}

FsSession   *Stream::sessionElement()
{
    gst_object_ref(m_session);
    return m_session;
}

FsStream    *Stream::streamElement()
{
    gst_object_ref(m_stream);
    return m_stream;
}

/** @brief Get farsight object of participant for this stream. */
FsParticipant *Stream::participant()
{
    FsParticipant *p = NULL;
    g_object_get(G_OBJECT(m_stream), "participant", &p, NULL);
    return p;
}

/** @brief Get name of participant of this stream. */
std::string Stream::participantName()
{
    FsParticipant *p = participant();
    gchar *name = NULL;
    g_object_get(G_OBJECT(p), "cname", &name, NULL);
    std::string pname(name);
    g_free(name);
    return pname;
}

