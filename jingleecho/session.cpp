
#include <iostream>
#include <gst/gstpad.h>
#include "conference.h"

Session::Session(Conference *conf)
    : m_conf(conf),m_session(NULL),m_lasterror(NULL),m_stream(NULL),
      m_localCandidates(NULL)
{
    GError *err =NULL;
    m_session = fs_conference_new_session(FS_CONFERENCE(conf->conference()), FS_MEDIA_TYPE_AUDIO, &err);
    if (err != NULL) {
        std::cerr <<__FUNCTION__<< " fs_conference_new_session: "<< err->message << std::endl;
    }
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
    FsStream *stream = fs_session_new_stream(
        m_session,
        participant,
        FS_DIRECTION_BOTH,
        "rawudp",
        paramcount, param, &m_lasterror);
    g_assert(stream);
    g_signal_connect(stream, "src-pad-added", G_CALLBACK(Session::srcPadAdded), this);
    return stream;
}

bool Session::createStream()
{
    GParameter param[1] = { {0},};
    int paramcount = 0;

    if (m_localCandidates) {
        param[paramcount].name = "preferred-local-candidates";
        g_value_init(&param[paramcount].value, FS_TYPE_CANDIDATE_LIST);
        g_value_take_boxed(&param[paramcount].value, m_localCandidates);
        paramcount++;
    }

    FsStream *stream = fs_session_new_stream(
        m_session,
        m_conf->remoteParticipant(),
        FS_DIRECTION_BOTH,
        "rawudp",
        paramcount, param, &m_lasterror);
    if (m_lasterror) {
        std::cerr << __FUNCTION__ << " fs_session_new_stream:" 
                  << m_lasterror->message << std::endl;
    }
    g_assert(stream);
    m_stream = stream;
    g_signal_connect(m_stream, "src-pad-added", G_CALLBACK(Session::srcPadAdded), this);
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
}

void Session::setLocalCodec(GList *codecs)
{
    g_assert(m_session);
    fs_session_set_codec_preferences(m_session, codecs, &m_lasterror);
}

bool Session::setRemoteCodec(GList *codecs)
{
    if (m_stream) {
        return fs_stream_set_remote_codecs(m_stream, codecs, &m_lasterror);
    }
    return false;
}

/** @brief Returns sink pad, where should be sended data sent to.
    You have to unref this pad when not needed. */
GstPad * Session::sink()
{
    GstPad *sink = NULL;
    g_object_get(m_session, "sink-pad", &sink, NULL);
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
    std::cout << "Source pad added" << std::endl;
    session->m_conf->srcPadAdded(session, pad, codec);
}


