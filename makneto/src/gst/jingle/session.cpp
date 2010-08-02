
#include <iostream>
#include <sstream>
#include <cstdlib>

#include <gst/gstpad.h>
#include "stream.h"
#include "conference.h"

#include "logger/logger.h"
#include "fstjingle.h"

using namespace farsight;

/** @brief Create one media stream session.
    @param conf Conference class, to which it will belong.
    @param type Type of media transmitted with this session.
*/
Session::Session(Conference *conf, FsMediaType type)
    : m_conf(conf),m_session(NULL),m_lasterror(NULL),
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

Session::~Session()
{
    fs_candidate_list_destroy(m_localCandidates);
    g_object_unref(m_session);
}

Conference * Session::conference()
{
    return m_conf;
}

/** @brief Create new stream for this session. */
Stream *Session::createStream(FsParticipant *participant, const GList *lcandidates)
{
    Stream *s = new Stream(this, participant, lcandidates);
    m_conf->streamCreated(this, s);
    return s;
}

/** @brief Create stream for one participant. 
    @return true if stream was created successfully, false otherwise. */
bool Session::createStream(FsParticipant *participant)
{
    Stream *s = createStream(participant, m_localCandidates);
    m_streams.push_back(s);
    return (s != NULL);
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
    return (m_streams.size() > 0);
}

void Session::setRemote(const std::string &ip, int port)
{
    if (m_streams.size() > 0)
        m_streams.front()->setRemote(ip, port);
}

/** @brief set remote candidates from GList of (FsCandidate *) */
void Session::setRemote(GList *list)
{
    if (m_streams.size() > 0)
        m_streams.front()->setRemote(list);
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
    if (firstStream()) {
        firstStream()->setRemoteCodec(codecs);
        return true;
    } else {
        return false;
    }
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

/** @brief Configure local port, any address on host will be used. */
void Session::setLocalPort(unsigned int port)
{
    const gchar *foundation = "foundation";
    if (m_localCandidates)
        fs_candidate_list_destroy(m_localCandidates);
    FsCandidate * candidate = fs_candidate_new(foundation, 1, 
        FS_CANDIDATE_TYPE_HOST, FS_NETWORK_PROTOCOL_UDP,
        NULL, port);
    m_localCandidates = g_list_prepend(NULL, candidate);
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

#if 0
GList * Session::currentRecvCodec(const std::string &participant)
{
    Stream *s = NULL;
    if (participant.empty()) 
        s = firstStream();
    else
        s = getStream(participant);
    if (s)
        return s->currentRecvCodec();
    else 
        return NULL;
}
#endif

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
    o << "Streams: " << std::endl;
    for (StreamList::const_iterator it=m_streams.begin(); it!=m_streams.end(); it++) {
        o << (*it)->describe() << std::endl;
    }

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

GList * Session::getNewLocalCandidates()
{
    return fs_candidate_list_copy(m_newLocalCandidates);
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
    m_newLocalCandidates = g_list_prepend(m_newLocalCandidates, copy);
}

FsSession   *Session::sessionElement()
{
    gst_object_ref(m_session);
    return m_session;
}

FsStream    *Session::streamElement()
{
    if (firstStream())
        return firstStream()->streamElement();
    return NULL;
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

/** @brief Get first stream of this session. */
Stream *Session::firstStream()
{
    if (m_streams.empty())
        return NULL;
    else
        return m_streams.front();
}

/** @brief Find stream class by participant name. 
    @param participant Name of participant, if empty, any participant. */
Stream *Session::getStream(const std::string &participant)
{
    if (participant.empty())
        return firstStream();

    for (StreamList::iterator it=m_streams.begin(); it!=m_streams.end(); it++) {
        if (participant == (*it)->participantName())
            return (*it);
    }
    return NULL;
}

/** @brief Find stream class by farsight stream class.
    Use this on messages callback. */
Stream *Session::getStream(FsStream *stream)
{
    for (StreamList::iterator it=m_streams.begin(); it!=m_streams.end(); it++) {
        if (stream == (*it)->streamElement()) {
            gst_object_unref(GST_OBJECT(stream));
            return (*it);
        }
        gst_object_unref(GST_OBJECT(stream));
    }
    return NULL;
}

StreamList Session::streams()
{
    return m_streams;
}

/** @brief Return number of streams this session have active. */
int Session::streamCount()
{
    return m_streams.size();
}

