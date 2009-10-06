
#include "conference.h"

Session::Session(Conference *conf)
    m_conf(conf),m_lasterror(NULL)
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

