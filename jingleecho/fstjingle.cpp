
#include <iostream>
#include <sstream>
#include <glib.h>

#include "fstjingle.h"
#include "logger.h"


FstJingle::FstJingle()
{
    pipeline = new QPipeline();
    conference = new Conference(pipeline);
}

FstJingle::~FstJingle()
{
    delete pipeline;
    delete conference;
}

/** @brief Convert jingle structure to farsight format */
FsCandidate * FstJingle::createFsCandidate(const JingleCandidate & candidate)
{
    const char foundation[] = "";
    FsCandidateType type = FS_CANDIDATE_TYPE_HOST;
    switch (candidate.candidateType) {
        case JingleCandidate::TYPE_HOST:
            type = FS_CANDIDATE_TYPE_HOST;  break;
        case JingleCandidate::TYPE_REFLEXIVE:
            type = FS_CANDIDATE_TYPE_SRFLX; break;
        case JingleCandidate::TYPE_RELAYED:
            type = FS_CANDIDATE_TYPE_RELAY; break;
        default: 
            break;
    }
    FsCandidate *fscan = fs_candidate_new(foundation,
        candidate.component, type, FS_NETWORK_PROTOCOL_UDP,
        candidate.ip.c_str(), candidate.port);
    return fscan;
}

JingleCandidate FstJingle::createJingleCandidate(const FsCandidate *candidate)
{
    JingleCandidate jc;

    if (candidate->ip)
        jc.ip = candidate->ip;
    jc.port = candidate->port;
    //jc.foundation = candidate->foundation;
    jc.component = candidate->component_id;
    switch (candidate->type) {
        case FS_CANDIDATE_TYPE_HOST:
            jc.candidateType = JingleCandidate::TYPE_HOST; break;
        case FS_CANDIDATE_TYPE_SRFLX:
        case FS_CANDIDATE_TYPE_PRFLX:
            jc.candidateType = JingleCandidate::TYPE_REFLEXIVE; break;
        case FS_CANDIDATE_TYPE_RELAY:
            jc.candidateType = JingleCandidate::TYPE_RELAYED; break;
        default:
            jc.candidateType = JingleCandidate::TYPE_UNKNOWN; break;
    }
    return jc;
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
    for (CandidateList::const_reverse_iterator 
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
    g_assert(session);
    session->setName(remote.name());
    
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
    setNicknames(session->from().fullStd(), session->to().fullStd());
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

/** @brief Replace previous content with this content. */
bool FstJingle::replaceRemoteContent(const JingleContent &content)
{
    GList *remoteCandidates = createFsCandidateList(content.transport());
    GList *remoteCodecs = createFsCodecList(content.description());

    Session *session = conference->getSession(content.name());
    if (session) {
        session->setRemote(remoteCandidates);
        session->setRemoteCodec(remoteCodecs);
        return true;
    } else {
        LOGGER(logit) << "Content na nahrazeni nema odpovidajici session!" 
            << content.name() << " media " << content.media() 
            << std::endl;
        return false;
    }
}

bool FstJingle::replaceRemoteCandidate(const std::string &content, const JingleCandidate &candidate)
{
    Session *session = conference->getSession(content);
    if (session) {
        GList *candidates = NULL;
        FsCandidate *fscan = createFsCandidate(candidate);
        candidates = g_list_prepend(candidates, fscan);
        session->setRemote(candidates);
        fs_candidate_list_destroy(candidates);
        LOGGER(logit) << "Nahrazuji vzdaleneho kandidata " 
                << fscan->ip << ":" 
                << fscan->port << " "
                << "na content: " << content << std::endl;
        return true;
    } else {
        LOGGER(logit)
        << "Nenalezena session pri nahrazovani vzdaleneho kandidata: " 
        << content << std::endl;
    }
    return false;
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

/** @brief Terminate current session. */
bool FstJingle::terminate()
{
    conference->removeAllSessions();
    pipeline->setState(GST_STATE_NULL);
    return true;
}

bool FstJingle::isPlaying()
{
    return (pipeline->current_state() == GST_STATE_PLAYING);
}

bool FstJingle::isPaused()
{
    return (pipeline->current_state() == GST_STATE_PAUSED);
}

bool FstJingle::isReady()
{
    return (pipeline->current_state() == GST_STATE_READY);
}

bool FstJingle::haveLocalCandidates()
{
    return conference->haveLocalCandidates();
}

CandidateList   FstJingle::localCandidates()
{
    GList *candidates = conference->localCandidates();
    GList *item = candidates;
    CandidateList cl;

    while(item) {
        cl.push_back(createJingleCandidate((FsCandidate *) item->data));
        item = g_list_next(item);
    }
    fs_candidate_list_destroy(candidates);
    return cl;
}

/** @brief Update local transport and its candidates from information we know
    about. 
    @param content Content to update.
    @return True if updated, false otherwise. 
*/
bool FstJingle::updateLocalTransport(JingleContent &content)
{
    Session *fs = conference->getSession(content.name());
    if (fs) {
        GList * candidates = fs->getLocalCandidates();
        CandidateList cl = createJingleCandidateList(candidates);
        fs_candidate_list_destroy(candidates);
        JingleTransport jt = content.transport();
        jt.candidates = cl;
        content.setTransport(jt);
        return true;
    }
    return false;
}
    
bool FstJingle::haveNewLocalCandidates()
{
    return conference->haveNewLocalCandidates();
}

void FstJingle::resetNewLocalCandidates()
{
    conference->resetNewLocalCandidates();
}

CandidateList   FstJingle::createJingleCandidateList(GList *candidates)
{
    CandidateList cl;
    while (candidates) {
        cl.push_back(createJingleCandidate((FsCandidate *) candidates->data));
        candidates = g_list_next(candidates);
    }
    return cl;
}

/** @brief Try next candidate for this content. 
    @return true if there is next candidate to try, false otherwise.
*/
bool FstJingle::tryNextCandidate(JingleContent &content)
{
    bool modified = false;
    bool next = false;
    JingleTransport jt = content.transport();
    for (CandidateList::iterator ci = jt.candidates.begin(); 
                ci!= jt.candidates.end(); ci++) 
    {
        if (ci->reachable == JingleCandidate::REACHABLE_TRYING) {
            ci->reachable = JingleCandidate::REACHABLE_NO;
            LOGGER(logit) << "Candidate marked unreachable" << std::endl;
            modified = true;
        } else if (ci->reachable == JingleCandidate::REACHABLE_UNKNOWN) {
            ci->reachable = JingleCandidate::REACHABLE_TRYING;
            LOGGER(logit) << "Candidate marked trying " << std::endl;
            modified = true;
            replaceRemoteCandidate(content.name(), *ci);
            next = true;
        }
    } // for candidates
    if (modified)
        content.setTransport(jt);
    return (modified && next);
}


