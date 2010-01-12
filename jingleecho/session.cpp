
#include <gst/gstpad.h>
#include "conference.h"

Session::Session(Conference *conf)
    m_conf(conf),m_lasterror(NULL),m_session(NULL),m_stream(NULL)
{
}

FsStream *Session::createStream(FsParticipant *participant)
{
    FsStream *stream = fs_session_new_stream(
        m_session,
        participant,
        FS_DIRECTION_BOTH,
        "rawudp",
        0, NULL, &m_lasterror);
    return stream;
}

void Session::setRemote(const std::string &ip, int port)
{
    const gchar *foundation = "foundation-test";

    FsCandidate * candidate = fs_candidate_new(foundation, 1, 
        FS_CANDIDATE_HOST, FS_NETWORK_PROTOCOL_UDP,
        ip.c_str(), port);

    GList *candidates = g_list_prepend(NULL, candidate);

    fs_stream_set_remote_candidates(m_stream, candidates, &m_lasterror);

    fs_candidate_list_destroy(candidates);
}

/** @brief set remote candidates from GList of (FsCandidate *) */
void Session::setRemote(const GList *list)
{
    fs_stream_set_remtote_candidates(m_session, candidates, &m_lasterror);
}

void Session::setLocalCodec(const GList *codecs)
{
    fs_session_set_codec_preferences(m_session, codecs, &m_lasterror);
}

bool Session::setRemoteCodec(const GList *codecs)
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
    GValue val;
    g_value_init(&val, gst_pad_get_type());
    g_object_get_property(G_OBJECT(m_session), "sink-pad", &val);
    sink = GST_PAD(g_value_get_object(&val));
    return sink;
}

