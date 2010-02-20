
#include <iostream>
#include <sstream>
#include <glib.h>

#include "fstjingle.h"
#include "logger.h"

/** @brief Create farsight abstraction class.
    @param reader FstStatusReader pointer to class wanting messages about farsight pipeline, or NULL if not needed. */
FstJingle::FstJingle(JingleSession *js, FstStatusReader *reader)
    : m_lastErrorCode(NoError)
{
    m_reader = reader;
    pipeline = new QPipeline();
    if (!pipeline || !pipeline->isValid()) {
        LOGGER(logit) << "Pipeline failed." << std::endl;
        setError(PipelineError);
        if (reader) {
            reader->reportMsg(FstStatusReader::MSG_FATAL_ERROR, "Pipeline creation failed.");
            reader->reportState(FstStatusReader::S_FAILED);
        }
        return;
    }

    if (!pipeline->enableAudio()) {
        LOGGER(logit) << "Audio pipeline failed." << std::endl;
        setError(PipelineError);
        if (reader) {
            reader->reportMsg(FstStatusReader::MSG_FATAL_ERROR, "Audio pipeline elements creation failed.");
            reader->reportState(FstStatusReader::S_FAILED);
        }
        return;
    }

    conference = new Conference(pipeline);
    if (!conference || (conference->lastError() != NoError)) {
        if (reader) {
            reader->reportMsg(FstStatusReader::MSG_ERROR, conference->lastErrorMessage());
            reader->reportState(FstStatusReader::S_FAILED);
        }
    }

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

/** @brief Create internal candidate structure from Farsight candidate. */
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

/** @brief Create internal candidate representation.
    @param candidate Farsight candidate structure
    @param xmlns Transport namespace. One of XMLNS_JINGLE_ICE or XMLNS_JINGLE_RAWUDP.
    @return Candidate in internal structure.
*/
JingleCandidate FstJingle::createJingleIceCandidate(const FsCandidate *candidate, const std::string &xmlns)
{
    if (xmlns == XMLNS_JINGLE_ICE) {
        JingleIceCandidate jc = createJingleCandidate(candidate);

        switch(candidate->proto) {
            case FS_NETWORK_PROTOCOL_UDP:
                jc.protocol = JingleIceCandidate::PR_UDP;
                break;
            case FS_NETWORK_PROTOCOL_TCP:
                jc.protocol = JingleIceCandidate::PR_TCP;
                break;
        }

        jc.priority = candidate->priority;
        jc.relAddr = candidate->base_ip;
        jc.relPort = candidate->base_port;

        return jc;
    } else if (xmlns == XMLNS_JINGLE_RAWUDP) {
        return createJingleCandidate(candidate);
    } else {
        LOGGER(logit) << "Vyzadana tvorba candidate pro nepodporovane XMLNS: "
                << xmlns << std::endl;
        return JingleCandidate();
    }
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

/** @brief Link network sink of session to source of choosen type.
    @param session FsSession wrap which sink it to be connected to source.
    @param type Type of source to connect to sink. 
*/
bool FstJingle::linkSink(Session *session, FsMediaType type)
{
    GstPad *src = NULL;
    
    switch (type) {
        case FS_MEDIA_TYPE_AUDIO:
            src = pipeline->getAudioSourcePad();
            break;
        case FS_MEDIA_TYPE_VIDEO:
            src = pipeline->getVideoSourcePad();
            break;
    }
    if (!src) {
        setError(PipelineError, "Source pad is NULL");
        return false;
    }

    GstPad *sink = session->sink();
    if (!sink) {
        setError(PipelineError, "Sink pad is NULL");
        return false;
    }
    GstPadLinkReturn r = gst_pad_link(src, sink);
    if (GST_PAD_LINK_FAILED(r)) {
        LOGGER(logit) << "pad link zdroje selhal!" << r << std::endl;
    }
    
    GstCaps *caps = gst_pad_get_caps(sink);
    gchar * capsstr = gst_caps_to_string(caps);
    LOGGER(logit) << "fs sink has caps: " << capsstr << std::endl;
    g_free(capsstr);
    gst_object_unref(src);
    gst_caps_unref(caps);
    gst_object_unref(sink);
    return (GST_PAD_LINK_SUCCESSFUL(r));
}

/** @brief create audio session in farsight. */
bool FstJingle::createAudioSession(const JingleContent &local, const JingleContent &remote)
{
    MediaType mtype = local.description().type();
    FsMediaType fstype = FS_MEDIA_TYPE_AUDIO;
    switch (mtype) {
        case MEDIA_AUDIO:
            fstype = FS_MEDIA_TYPE_AUDIO; 
            if (!pipeline->isAudioEnabled()) {
                if (!pipeline->enableAudio()) {
                    LOGGER(logit) << "enableAudio failed." << std::endl;
                    setError(PipelineError, "pipeline enableAudio failed.");
                    return false;
                }
            }
            break;
        case MEDIA_VIDEO:
            fstype = FS_MEDIA_TYPE_VIDEO; 
            if (!pipeline->isVideoEnabled()) {
                if (!pipeline->enableVideo()) {
                    LOGGER(logit) << "enableVideo failed." << std::endl;
                    setError(PipelineError, "pipeline enableVideo failed.");
                    return false;
                }
            }
            break;
        case MEDIA_NONE:
            LOGGER(logit) << "requested creation of content with local type NONE" << std::endl;
            break;
    }

    Session *session = new Session(conference, fstype);
    g_assert(session);
    session->setName(remote.name());
    
    GList *remoteCandidates = createFsCandidateList(remote.transport());
    GList *localCandidates = createSingleFsCandidateList(local.transport());

    session->setLocal(localCandidates);
    bool created = session->createStream();
    session->setRemote(remoteCandidates);

    GList *remoteCodecs = createFsCodecList(remote.description());
    session->setRemoteCodec(remoteCodecs);

    bool linked = linkSink(session, fstype);

    conference->addSession(session);
    LOGGER(logit) << "Created session: " << remote.name() 
        << " type: " << fstype
        << " created: " << created
        << " linked: " << linked
        << std::endl;
    return (created && linked);
}

/** @brief Configure farsight session from passed JingleSession structure. */
bool FstJingle::createAudioSession(JingleSession *session)
{
    bool result = true;
    bool empty = true;
    setNicknames(session->from().fullStd(), session->to().fullStd());
    ContentList lcl = session->localContents();
    ContentList rcl = session->remoteContents();
    ContentList::iterator lit = lcl.begin();
    ContentList::iterator rit = rcl.begin();

    while (lit != lcl.end()) {
        bool found = false;
        if (empty) {
            std::string xmlns = rit->transport().xmlns();
            std::string tr = xmlnsToTransmitter(xmlns);
            if (!tr.empty()) 
                conference->setTransmitter(tr);
        }

        for (rit = rcl.begin(); rit != rcl.end(); rit++) {
            // match by same types
            // TODO: should i match by names?
            if (lit->description().type() == rit->description().type()) {
                found = true;
                result = result && createAudioSession(*lit, *rit);
                empty = false;
            }
        }
        ++lit;
    } 
    if (empty)
        LOGGER(logit) << "empty audio session created." << std::endl;

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
    return result && paused && playing && !empty;
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
        LOGGER(logit) << "Content na nahrazeni nema odpovidajici session! " 
            << content.name() << " media " << content.media() 
            << std::endl;
        return false;
    }
}

/** @brief Replace remote candidate for specified content.
    @param content Name of content to be replaced.
    @param candidate New candidate that will be used.
*/
bool FstJingle::replaceRemoteCandidate(const std::string &content, const JingleCandidate &candidate)
{
    Session *session = conference->getSession(content);
    if (session) {
        GList *candidates = NULL;
        FsCandidate *fscan = createFsCandidate(candidate);
        candidates = g_list_prepend(candidates, fscan);
        session->setRemote(candidates);
        // TODO: lepsi spravu zdroju, tohle je primo prirazeno do session, nelze uvolnit.
        // fs_candidate_list_destroy(candidates);
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
   
/** @brief Indicate, if there is new candidates since last reset. */ 
bool FstJingle::haveNewLocalCandidates()
{
    return conference->haveNewLocalCandidates();
}

/** @brief Confirm you have read current candidates. */
void FstJingle::resetNewLocalCandidates()
{
    conference->resetNewLocalCandidates();
}

/** @brief Return autodetected candidates for this host. */
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
    It will mark current candidate unreachable, and mark next candidate
    trying. If no more candidates, return false. 
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
            LOGGER(logit) << "Candidate marked unreachable" 
                << ci->ip << std::endl;
            modified = true;
        } else if (ci->reachable == JingleCandidate::REACHABLE_UNKNOWN && !next) {
            ci->reachable = JingleCandidate::REACHABLE_TRYING;
            LOGGER(logit) << "Candidate marked trying "
                << ci->ip << std::endl;
            modified = true;
            replaceRemoteCandidate(content.name(), *ci);
            next = true;
        }
    } // for candidates
    if (modified)
        content.setTransport(jt);
    return (modified && next);
}

/** @brief Configure stun server IP address and port.
    It has to be IP address, not general hostname. 
    @param ip IP address in text format, dotted quad format for IPv4. 
    @param port Port of stun server. If 0, default stun port will be used. */
void FstJingle::setStun(const std::string &ip, int port)
{
    conference->setStun(ip, port);
}


void FstJingle::setError(JingleFarsightErrors e, const std::string &msg)
{
    m_lastErrorCode = e;
    m_lastErrorMessage = msg;
}

JingleFarsightErrors FstJingle::lastError()
{
    JingleFarsightErrors e = conference->lastError();
    if (e == NoError)
        return m_lastErrorCode;
    return e;
}

std::string FstJingle::lastErrorMessage()
{
    std::string msg = conference->lastErrorMessage();
    if (msg.empty())
        return m_lastErrorMessage;
    else return msg;
}

/** @brief Convert transport namespace to name of transmitter farsight supports.
    @param xmlns XMLNS of transport element in jingle.
    It will be rawudp, nice, or empty for unknown. */
std::string FstJingle::xmlnsToTransmitter(const std::string &xmlns)
{
    if (xmlns == XMLNS_JINGLE_RAWUDP)
        return "rawudp";
    else if (xmlns == XMLNS_JINGLE_ICE)
        return "nice";
    else {
        LOGGER(logit) << "Nepodporovane xmlns pro transport: " << xmlns<<std::endl;
        return std::string();
    }
}

