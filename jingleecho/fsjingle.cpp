
#include <glib.h>

#include "fsjingle.h"



FstJingle::FstJingle()
{
    pipeline = new QPipeline();
    conference = new Conference(pipeline);
}

/** @brief Convert jingle structure to farsight format */
FsCandiate * FstJingle::createFsCandidate(const JingleCandidate & candidate)
{
    const char foundation[] = "";
    FsCandidate *fscan = fs_candidate_new(
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

    if (!description.media.empty()) {
        if (description.media == "audio")
            mediatype = FS_MEDIA_TYPE_AUDIO;
        else if (description.media == "video")
            mediatype = FS_MEDIA_TYPE_VIDEO;
    }

    for (JingleRtpContentDescription::PayloadList::iterator 
            it= description.rbegin();
            it != description.rend(); it++) {
        FsCodec *codec = fs_codec_new(
            it->id, it->name.c_str(), mediatype, it->clockrate);
        g_list_prepend(codeclist, codec);
    }
    return codeclist;
}

/** @brief Create list of candidates from transport structure. */
GList * FstJingle::createFsCandidateList(const JingleTransport &transport)
{
    GList *candidates = NULL;
    for (JingleTransport::CandidateList::iterator 
            it = transport.candidates.rbegin();
            it != transport.candidates.rend();
            it++) {
        FsCandidate *candidate = createFsCandidate(*it);
        g_list_prepend(candidates, candidate);
    }
    return candidates;
}

/** @brief create audio session in farsight. */
bool FstJingle::createAudioSession(const JingleContent &local, const JingleContent &remote)
{
    Session *session = new Session(conference);
    
    session->setLocal(localCandidates);
    session->createStream();
    session->setRemote(remoteCandidates);

    GList *localCodecs = createFsCodecList(local.description());
    session->setLocalCodec(localCodec);

    GList *remoteCodecs = createFsCodecList(remote.description());
    session->setRemoteCodec(remoteCodecs);

    GList *remoteCandidates = createFsCandidateList(remote.transport());
    GList *localCandidates = createFsCandidateList(local.transport());

    conference.addSession(session);
}

