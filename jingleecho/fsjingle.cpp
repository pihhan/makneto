
#include <iostream>
#include <sstream>
#include <glib.h>

#include "fsjingle.h"
#include "logger/logger.h"


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

/** @brief Create list containing only one, topmost candidate.
    Modified, removes IP from candidates, so only port applies.  */
GList * FstJingle::createSingleFsCandidateList(const JingleTransport &transport)
{
    GList *candidates = NULL;
    if (transport.candidates.size()>0) {
        JingleCandidate c = transport.candidates.front();
        FsCandidate *candidate = createFsCandidate(c);
        // FIXME: specialni fix, aby fungoval prijem na vsech adresach
        candidate->ip = NULL;
        candidates = g_list_prepend(candidates, candidate);
        return candidates;
    } else {
        return NULL;
    }
}

bool FstJingle::linkSink(Session *session)
{
    GstPad *audiosrc = pipeline->getAudioSourcePad();
    g_assert(audiosrc);
#if 0
    GstCaps *srccaps = gst_pad_get_caps(audiosrc);
    GstCaps *fixed = gst_caps_new_simple("audio/x-raw-int",
        "rate", G_TYPE_INT, 8000,
        "channels", G_TYPE_INT, 1,
        "width", G_TYPE_INT, 16,
        NULL);
    g_assert(fixed);
    GstCaps *intersect = gst_caps_intersect(srccaps, fixed);
    g_assert(intersect);
    if (!gst_caps_is_fixed(intersect)) {
        LOGGER(logit) << "intersect neni fixed" << std::endl;
    }
    gst_pad_set_caps(audiosrc, intersect);
    g_object_unref(fixed);
    g_object_unref(srccaps);
    g_object_unref(intersect);
#endif

    GstPad *sink = session->sink();
    g_assert(audiosrc && sink);
    GstPadLinkReturn r = gst_pad_link(audiosrc, sink);
    if (GST_PAD_LINK_FAILED(r)) {
        LOGGER(logit) << "pad link zdroje selhal!" << r << std::endl;
    }
    
    GstCaps *caps = gst_pad_get_caps(sink);
    gchar * capsstr = gst_caps_to_string(caps);
    LOGGER(logit) << "fs sink has caps: " << capsstr << std::endl;
    g_free(capsstr);
    gst_object_unref(audiosrc);
    gst_caps_unref(caps);
    gst_object_unref(sink);
    return (GST_PAD_LINK_SUCCESSFUL(r));
}

/** @brief create audio session in farsight. */
bool FstJingle::createAudioSession(const JingleContent &local, const JingleContent &remote)
{
    Session *session = new Session(conference);
    
    GList *remoteCandidates = createFsCandidateList(remote.transport());
    GList *localCandidates = createSingleFsCandidateList(local.transport());

    session->setLocal(localCandidates);
    session->createStream();
    session->setRemote(remoteCandidates);

    GList *localCodecs = createFsCodecList(local.description());
    session->setLocalCodec(localCodecs);

    GList *remoteCodecs = createFsCodecList(remote.description());
    session->setRemoteCodec(remoteCodecs);

    linkSink(session);

    conference->addSession(session);
    LOGGER(logit) << "created Audio session" << std::endl;
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
    LOGGER(logit) << "createAudioSession result: " << result << std::endl;

    bool paused = pipeline->setState(GST_STATE_PLAYING);
    LOGGER(logit) << "Paused pipeline: " << paused << std::endl;

    LOGGER(logit) << "Pipeline state " << pipeline->current_state() 
        << " pending " << pipeline->pending_state() << std::endl;

    bool playing = true;
#if 0 
    playing = pipeline->setState(GST_STATE_PLAYING);
    LOGGER(logit) << "Playing pipeline: " << playing << std::endl;
    LOGGER(logit) << "Pipeline state " << pipeline->current_state() 
        << " pending " << pipeline->pending_state() << std::endl;
#endif
    pipeline->describe();
    return result && paused && playing;
}

bool FstJingle::replaceRemoteContent(const JingleContent &content)
{
    GList *remoteCandidates = createFsCandidateList(remote.transport());
    GList *remoteCodecs = createFsCodecList(remote.description());

    session->setRemote(remoteCandidates);
    session->setRemoteCodec(remoteCodecs);
    return true;
}


void FstJingle::setNicknames(const std::string &local, const std::string &remote)
{
    conference->setNicknames(local, remote);
}

std::string FstJingle::stateDescribe()
{
    std::ostringstream o;
    o << pipeline->describe() << std::endl;
    o << conference->describe() << std::endl;
    return o.str();
}

std::string FstJingle::codecListToString(const GList *codeclist)
{
    std::string description;
    bool first = true;
    if (!codeclist)
        return description;
    while (codeclist) {
        FsCodec * codec = (FsCodec *) codeclist->data;
        if (!first)
            description += ", ";
        description += toString(codec);
        first = false;
        codeclist = g_list_next(codeclist);
    }
    return description;
}

std::string FstJingle::toString(const FsCodec *codec)
{
    gchar * desc = fs_codec_to_string(codec);
    std::string cppdesc(desc);
    g_free(desc);
    return cppdesc;
}


