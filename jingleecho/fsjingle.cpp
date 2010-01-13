
#include <glib.h>

#include "fsjingle.h"



FstJingle::FstJingle()
{
    pipeline = new QPipeline();
    conference = new Conference(pipeline);
}

/** @brief Convert jingle structure to farsight format */
FsCandidate * FstJingle::createFsCandidate(const JingleCandidate & candidate)
{
    const char foundation[] = "";
    FsCandidate *fscan = fs_candidate_new(foundation,
        candidate.component, FS_CANDIDATE_TYPE_HOST, FS_NETWORK_PROTOCOL_UDP,
        candidate.ip.c_str(), candidate.port);
    return fscan;
}

/** @brief Convert jingle rtp description to farsight codec.

    For now, it always create audio content. */
FsCodec * FstJingle::createFsCodec(const JingleRtpPayload & payload)
{
    // TODO: pridat media type do payload!
    FsCodec *codec = fs_codec_new(payload.id, payload.name.c_str(),
            FS_MEDIA_TYPE_AUDIO, payload.clockrate);
    return codec;
}

/** @brief Create list of codecs from media description. */
GList * FstJingle::createFsCodecList(const JingleRtpContentDescription &description)
{
    GList *codeclist = NULL;
    FsMediaType mediatype = FS_MEDIA_TYPE_AUDIO;

    if (!description.media().empty()) {
        if (description.media() == "audio")
            mediatype = FS_MEDIA_TYPE_AUDIO;
        else if (description.media() == "video")
            mediatype = FS_MEDIA_TYPE_VIDEO;
    }

    for (JingleRtpContentDescription::PayloadList::const_reverse_iterator 
            it= description.payloads.rbegin();
            it != description.payloads.rend(); it++) {
        FsCodec *codec = fs_codec_new(
            it->id, it->name.c_str(), mediatype, it->clockrate);
        codeclist = g_list_prepend(codeclist, codec);
    }
    return codeclist;
}

/** @brief Create list of candidates from transport structure. */
GList * FstJingle::createFsCandidateList(const JingleTransport &transport)
{
    GList *candidates = NULL;
    for (JingleTransport::CandidateList::const_reverse_iterator 
            it = transport.candidates.rbegin();
            it != transport.candidates.rend();
            it++) {
        FsCandidate *candidate = createFsCandidate(*it);
        candidates = g_list_prepend(candidates, candidate);
    }
    return candidates;
}

/** @brief create audio session in farsight. */
bool FstJingle::createAudioSession(const JingleContent &local, const JingleContent &remote)
{
    Session *session = new Session(conference);
    
    GList *remoteCandidates = createFsCandidateList(remote.transport());
    GList *localCandidates = createFsCandidateList(local.transport());

    session->setLocal(localCandidates);
    session->createStream();
    session->setRemote(remoteCandidates);

    GList *localCodecs = createFsCodecList(local.description());
    session->setLocalCodec(localCodecs);

    GList *remoteCodecs = createFsCodecList(remote.description());
    session->setRemoteCodec(remoteCodecs);

    conference->addSession(session);
    return true;
}

bool FstJingle::createAudioSession(JingleSession *session)
{
    bool result = true;
    setNicknames(session->from().full(), session->to().full());
    ContentList lcl = session->localContents();
    ContentList rcl = session->remoteContents();
    ContentList::iterator lit = lcl.begin();
    ContentList::iterator rit = rcl.begin();
    while (lit != lcl.end() && rit != rcl.end()) {
        result = result && createAudioSession(*lit, *rit);
        ++lit;
        ++rit;
    } 
    return result;
}

void FstJingle::setNicknames(const std::string &local, const std::string &remote)
{
    conference->setNicknames(local, remote);
}
