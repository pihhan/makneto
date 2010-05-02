
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
Stream::Stream(Session *session, FsParticipant *participant, const GList *lcandidates)
    : m_confsession(session),m_lasterror(0),m_stream(0),
      m_localCandidates(0), m_newLocalCandidates(0), m_state(S_NONE), m_js(0),
      m_output(0)
{
    m_stream = createStream(participant, lcandidates);
    g_signal_connect(m_stream, "src-pad-added", G_CALLBACK(Stream::srcPadAdded), this);
    g_signal_connect(m_stream, "error", G_CALLBACK(Stream::streamError), this);
}

Stream::~Stream()
{
    fs_candidate_list_destroy(m_localCandidates);
    g_object_unref(m_stream);
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

    Conference *conf = m_confsession->conference();

    if (lcandidates) {
        param[paramcount].name = "preferred-local-candidates";
        g_value_init(&param[paramcount].value, FS_TYPE_CANDIDATE_LIST);
        g_value_take_boxed(&param[paramcount].value, lcandidates);
        paramcount++;
    }
    if (!conf->stunIp().empty()) {
        param[paramcount].name = "stun-ip";
        g_value_init(&param[paramcount].value, G_TYPE_STRING);
        g_value_set_string(&param[paramcount].value, conf->stunIp().c_str());
        paramcount++;
    }
    if (conf->stunPort() > 0) {
        param[paramcount].name = "stun-port";
        g_value_init(&param[paramcount].value, G_TYPE_UINT);
        g_value_set_uint(&param[paramcount].value, conf->stunPort());
        paramcount++;
    }

    const gchar *transmitter = conf->transmitter().c_str();

    FsSession *fssession = m_confsession->sessionElement();
    FsStream *stream = fs_session_new_stream(
        fssession,
        participant,
        FS_DIRECTION_BOTH,
        transmitter,
        paramcount, param, &m_lasterror);
    if (m_lasterror) {
        LOGGER(logit) << " fs_session_new_stream: " 
                  << m_lasterror->message << std::endl;
    } else {
        LOGGER(logit) << "Created stream " << name() 
            << "with transmitter " << transmitter 
            << std::endl;
    }
    gst_object_unref(GST_OBJECT(fssession));
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
    const gchar *foundation = "foundation";
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
    g_assert(s);
    LOGGER(logit) << "Source pad added on stream " << s->name() << std::endl;
    s->session()->conference()->srcPadAdded(s->session(), pad, codec);
}

void Stream::streamError(FsStream *self, FsError errno, gchar *error_msg,
    gchar *debug_msg, gpointer user_data)
{
    Stream *s = (Stream *) user_data;
    LOGGER(logit).printf("Error on stream %p(%p): %d, %s, %s",
        self, s, errno, error_msg, debug_msg);
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

/** @brief Create text description of this stream and its state. */
std::string Stream::describe()
{
    std::ostringstream o("RTP stream ");
    o << " participant: " << participantName();
    o << " direction: " << direction();
    o << std::endl;

    o << " remote-codecs: " << FstJingle::codecListToString(getCodecListProperty("remote-codecs")) << std::endl;
    o << " negotiated-codecs: " << FstJingle::codecListToString(getCodecListProperty("negotiated-codecs"));
    o << " current-recv: " << FstJingle::codecListToString(getCodecListProperty("current-recv-codecs"));
    return o.str(); 
}

void Stream::resetError()
{
    if (m_lasterror) {
        g_error_free(m_lasterror);
    }
    m_lasterror = NULL;
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
    FsSession *m_session = m_confsession->sessionElement();
    return m_session;
}

FsStream    *Stream::streamElement()
{
    gst_object_ref(m_stream);
    return m_stream;
}

/** @brief Get farsight object of participant for this stream. */
FsParticipant *Stream::participant() const
{
    FsParticipant *p = NULL;
    g_object_get(G_OBJECT(m_stream), "participant", &p, NULL);
    return p;
}

/** @brief Get name of participant of this stream. */
std::string Stream::participantName() const
{
    FsParticipant *p = participant();
    gchar *name = NULL;
    g_object_get(G_OBJECT(p), "cname", &name, NULL);
    std::string pname(name);
    g_free(name);
    gst_object_unref(GST_OBJECT(p));
    return pname;
}

PipelineStateType Stream::state() const
{
    return m_state;
}

void Stream::setState(PipelineStateType state)
{
    m_state = state;
}

void Stream::setDirection(FsStreamDirection d)
{
    g_object_set(G_OBJECT(m_stream), "direction", d, NULL);
}

FsStreamDirection Stream::direction() const
{
    FsStreamDirection d = FS_DIRECTION_NONE;
    g_object_get(G_OBJECT(m_stream), "direction", &d, NULL);
    return d;
}

/** @brief Return name of this stream, created from name of session and 
    name of participant. */
std::string Stream::name() const
{
    return componentName() + "," + participantName();
}

std::string Stream::componentName() const
{
    return m_name;
}

void Stream::setComponentName(const std::string &name)
{
    m_name = name;
}


JingleSession * Stream::jingleSession() const
{
    return m_js;
}

void Stream::setJingleSession(JingleSession *js)
{
    m_js = js;
}

AVOutput * Stream::output() const
{
    return m_output;
}

void Stream::setOutput(AVOutput *out)
{
    m_output = out;
}

std::string Stream::sid() const
{
    return m_sid;
}

void Stream::setSid(const std::string &s)
{
    m_sid = s;
}


