
#include <iostream>
#include <sstream>
#include <vector>

#include <glib.h>

#include "fstjingle.h"
#include "stream.h"
#include "logger.h"

using namespace farsight;

/** @brief Create farsight abstraction class.
    @param reader FstStatusReader pointer to class wanting messages about farsight pipeline, or NULL if not needed. */
FstJingle::FstJingle(FstStatusReader *reader)
    : m_lastErrorCode(NoError), m_reader(reader), m_unconfiguredCodecs(false),
      m_state(S_PREPARING)
{
    pipeline = new QPipeline();
    if (!pipeline || !pipeline->isValid()) {
        LOGGER(logit) << "Pipeline failed." << std::endl;
        setError(PipelineError, "QPipeline is invalid.");
        if (m_reader) {
            m_reader->reportMsg(FstStatusReader::MSG_FATAL_ERROR, "Pipeline creation failed.");
            m_reader->reportState(S_FAILED);
        }
        return;
    }

    conference = new Conference(pipeline);
    if (!conference || (conference->lastError() != NoError)) {
        setError(PipelineError, "Conference is invalid.");
        if (m_reader) {
            m_reader->reportMsg(FstStatusReader::MSG_ERROR, conference->lastErrorMessage());
            m_reader->reportState(S_FAILED);
        }
    }

}

FstJingle::~FstJingle()
{
    if (conference)
        delete conference;
    if (pipeline)
        delete pipeline;
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
    std::string id;

    if (candidate->ip)
        jc.ip = candidate->ip;
    jc.port = candidate->port;
    std::ostringstream o;
    o << jc.ip << ":" << jc.port;
    jc.id = o.str(); // FIXME: generate better and random identifier
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
JingleCandidate FstJingle::createJingleIceCandidate(
    const FsCandidate *candidate, 
    const std::string &xmlns)
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
FsCodec * FstJingle::createFsCodec(const JingleRtpPayload & payload, FsMediaType type)
{
    // TODO: pridat media type do payload!
    FsCodec *codec = fs_codec_new(payload.id, payload.name.c_str(),
            type, payload.clockrate);
    for (JingleRtpPayload::ParameterList::const_iterator 
        it=payload.parameters.begin();
        it!=payload.parameters.end(); 
        it++) 
    {
        fs_codec_add_optional_parameter(
            codec,
            it->name().c_str(), 
            it->stringValue().c_str()
        );
    }
    return codec;
}

/** @brief Create list of codecs from media description. */
GList * FstJingle::createFsCodecList(const JingleRtpContentDescription &description)
{
    GList *codeclist = NULL;
    FsMediaType mediatype = FS_MEDIA_TYPE_AUDIO;

    switch (description.type()) {
        case MEDIA_AUDIO:
            mediatype = FS_MEDIA_TYPE_AUDIO; break;
        case MEDIA_VIDEO:
            mediatype = FS_MEDIA_TYPE_VIDEO; break;
        case MEDIA_NONE:
        case MEDIA_AUDIOVIDEO:
            if (!description.media().empty()) {
                if (description.media() == "audio")
                    mediatype = FS_MEDIA_TYPE_AUDIO;
                else if (description.media() == "video")
                    mediatype = FS_MEDIA_TYPE_VIDEO;
            }
            break;
    }

    for (PayloadList::const_reverse_iterator 
            it= description.payloads.rbegin();
            it != description.payloads.rend(); it++) {
        FsCodec *codec = createFsCodec(*it, mediatype);
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


/** @brief Create session for specified type. 
    @param type Type of session that will be created. 
    @param name Name of session, not necessary in future. */
bool FstJingle::createSession(MediaType type, const std::string &name)
{
    MediaType mtype = type;
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
            } else {
                LOGGER(logit) << "enableAudio succeed" << std::endl;
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
            } else {
                LOGGER(logit) << "enableVideo succeed" << std::endl;
            }
            break;
        case MEDIA_NONE:
        case MEDIA_AUDIOVIDEO:
            LOGGER(logit) << "requested creation of content with local type NONE" << std::endl;
            break;
    }

    Session *session = new Session(conference, fstype);
    if (!session)
        return false;
    session->setName(name);

    conference->addSession(session);

    bool linked = linkSink(session, fstype);
    if (!linked)
        setError(PipelineError, "Link of sink failed.");
    return (session != NULL && linked);
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


/** @brief Prepare session with type of local content passed. 
    This is called before we know anything about remote side,
    most important is to get list of local supported codecs. 
    Here is created FsSession class from conference. */
bool FstJingle::prepareSession(const JingleContent &local)
{
    MediaType mtype = local.description().type();
    FsMediaType fstype = FS_MEDIA_TYPE_AUDIO;
    switch (mtype) {
        case MEDIA_AUDIO:
            fstype = FS_MEDIA_TYPE_AUDIO; 
            break;
        case MEDIA_VIDEO:
            fstype = FS_MEDIA_TYPE_VIDEO; 
            break;
        case MEDIA_NONE:
        case MEDIA_AUDIOVIDEO:
            LOGGER(logit) << "requested creation of content with local type NONE" << std::endl;
            break;
    }

    Session *session = conference->getSession(fstype);
    if (!session) {
        if (!createSession(mtype, local.name() )) {
            LOGGER(logit) << "Creation of Session with type " << fstype 
                << "failed!" << std::endl;
            session->setName(local.name());
            setError(PipelineError, "Session creation failed.");
            return false;
        } else {
            session = conference->getSession(fstype);
            if (session) 
                LOGGER(logit) << "Session creation successful." << std::endl;
        }
    } else {
        LOGGER(logit) << "Session with type " << fstype     
            << " already exists." << std::endl;
    }

    GList *localCandidates = NULL;
    // rawudp transmitter cannot work with more than one concurrent 
    // local candidate
    if (local.transport().xmlns() == XMLNS_JINGLE_RAWUDP)
        localCandidates = createSingleFsCandidateList(local.transport());
    else 
        localCandidates = createFsCandidateList(local.transport());
    if (localCandidates)
        session->setLocal(localCandidates);

    return (session != NULL);
}

/** @brief Prepare all local contents for this session. 
    @return true if there are contents and was prepared, false if
        there is not content to prepare or it was not successful. */
bool FstJingle::prepareSession(JingleSession *session)
{
    ContentList cl = session->localContents();
    bool success = true;
    bool empty = true;
    
    setState(S_PREPARING);

    for (ContentList::iterator it = cl.begin(); it!=cl.end(); it++) {
        success = success && prepareSession(*it);
        if (!success) {
            LOGGER(logit) << "Failed to prepare session for content " 
                << it->name() << std::endl;
        } else {
            empty = false;
        }
    }

    return (success && !empty && goPlaying());
}



/** @brief Update remote settings for content, like new candidates or codecs. 
    @param remote Description and maybe candidates of remote session. 
    @param target Full JID of remote participant. */
bool FstJingle::updateRemote(
    const JingleContent &remote, 
    const std::string &target)
{
    FsMediaType ses_type = fsMediaType(remote.description().type());
    Session *session = conference->getSessionType(ses_type);
    AVOutput *output = conference->outputs().findOutput(target);
    if (session) {
        bool created;
        Stream *stream = session->getStream(target);
        bool exist = (stream != NULL);
        if (!stream) {
            FsParticipant *p = conference->getParticipant(target);
            created = session->createStream(p);
            if (!created) 
                return false;
            stream = session->getStream(target);
        }


        LOGGER(logit) << "Updating remote content for " 
            << remote.name() << std::endl;

        GList *remoteCandidates = createFsCandidateList(remote.transport());
        if (remoteCandidates != NULL) {
            stream->setRemote(remoteCandidates);
            LOGGER(logit) << "Updating remote candidates to: "
                << candidateListToString(remoteCandidates) << std::endl;
            fs_candidate_list_destroy(remoteCandidates);
        }

        GList *remoteCodecs = createFsCodecList(remote.description());
        if (remoteCodecs != NULL) {
            stream->setRemoteCodec(remoteCodecs);
            LOGGER(logit) << "Updating remote codecs to: "
                << codecListToString(remoteCodecs) << std::endl;
            fs_codec_list_destroy(remoteCodecs);
        }
        return (exist || created);
    } else {
        LOGGER(logit) << "Session not found for content: " 
                << remote.name() << std::endl;
        return false;
    }
}

/** @brief Update all remote content descriptions, candidates and codecs.
    @return true if it updated, false if there was problem. 
*/
bool FstJingle::updateRemote(JingleSession *session)
{
    if (!session)
        return false;
    ContentList cl = session->remoteContents();
    bool success = true;
    for (ContentList::iterator it=cl.begin(); it!=cl.end(); it++) {
        success = success && updateRemote(*it, session->remote().fullStd());
    }
    return success;
}

/** @brief create audio session in farsight. */
bool FstJingle::createAudioSession(const JingleContent &local, const JingleContent &remote)
{
    // FIXME: they must have same name
    bool prepared = prepareSession(local);
    bool updated = updateRemote(remote);

    LOGGER(logit) << "Created session: " << remote.name() 
        << " prepared: " << prepared
        << " updated: " << updated
        << std::endl;
    return (prepared && updated);
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
        if (!found) {
            LOGGER(logit) << "Local content " << lit->name()
                << " type " << lit->description().type() 
                << " does not have matching remote candidate." << std::endl;
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
    return result && paused && playing && !empty;
}


/** @brief Replace previous content with this content. */
bool FstJingle::replaceRemoteContent(const JingleContent &content, 
    const std::string &target)
{
    GList *remoteCandidates = createFsCandidateList(content.transport());
    GList *remoteCodecs = createFsCodecList(content.description());

    Session *session = conference->getSession(content.name());
    Stream *stream = conference->getStream(content.name(), target);
    if (session) {
        bool created = true;
        LOGGER(logit) << "Replacing remote content " << content.name() 
            << std::endl;
        if (!stream) {
            FsParticipant *p = conference->getParticipant(target);
            created = session->createStream(p);
            stream = session->getStream(target);
        }
        if (remoteCandidates) {
            stream->setRemote(remoteCandidates);
            LOGGER(logit) << "Replacing remote candidates to: " 
                << candidateListToString(remoteCandidates) << std::endl;
            fs_candidate_list_destroy(remoteCandidates);
        }
        bool cod = true;
        if (remoteCodecs) {
            cod = stream->setRemoteCodec(remoteCodecs);
            LOGGER(logit) << "Replacing remote codecs to: " 
                << codecListToString(remoteCodecs) << std::endl;
            fs_codec_list_destroy(remoteCodecs);
        }
        return (cod && created);
    } else {
        LOGGER(logit) << "Content to replace does not have existing session " 
            << content.name() << " media " << content.media() 
            << std::endl;
        return false;
    }
}

/** @brief Replace remote candidate for specified content.
    @param content Name of content to be replaced.
    @param candidate New candidate that will be used.
    @param target JID or nickname of participant
*/
bool FstJingle::replaceRemoteCandidate(
    const std::string &content, 
    const JingleCandidate &candidate, 
    const std::string &target)
{
    Session *session = conference->getSession(content);
    Stream *stream = conference->getStream(content, target);
    if (session) {
        bool exist = true;
        if (!stream) {
            FsParticipant *p = conference->getParticipant(target);
            exist = session->createStream(p);
            stream = session->getStream(target);
        }
        GList *candidates = NULL;
        FsCandidate *fscan = createFsCandidate(candidate);
        if (!fscan || !fscan->ip) {
            fs_candidate_destroy(fscan);
            LOGGER(logit) << "Skipping remote candidate without destination IP" << std::endl;
            return false;
        }
        candidates = g_list_prepend(candidates, fscan);
        if (candidates)
            stream->setRemote(candidates);
        LOGGER(logit) << "Replacing remote candidate to " 
                << ((fscan->ip) ? fscan->ip : "(any)") << ":" 
                << fscan->port << " "
                << "na content: " << content << std::endl;
        fs_candidate_list_destroy(candidates);
        return true;
    } else {
        LOGGER(logit)
        << "Farsight stream not found for content: " 
        << content << " target " << target << std::endl;
    }
    return false;
}


void FstJingle::setNicknames(const std::string &local, const std::string &remote)
{
    conference->setNicknames(local, remote);
}

/** @brief Get debug string with state of pipeline and conference. */
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

/** @brief Create string description of candidate list */
std::string FstJingle::candidateListToString(const GList *candidates)
{
    std::string description;
    bool first = true;
    if (!candidates)
        return description;
    while (candidates) {
        FsCandidate * c = (FsCandidate *) candidates->data;
        if (!first)
            description += ", ";
        description += toString(c);
        first = false;
        candidates = g_list_next(candidates);
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

/** @brief Create description for candidate. */
std::string FstJingle::toString(const FsCandidate *candidate)
{
    std::ostringstream o;
    if (!candidate)
        return std::string();
    o << "Candidate ";
    if (candidate->ip)
        o << "ip: " << candidate->ip << ":";
    else 
        o << "ip: any:";
    o << candidate->port << " ";
    if (candidate->foundation)
        o << "foundation: " << candidate->foundation << " ";
    o << "component: " << candidate->component_id << " ";
    if (candidate->base_ip) {
        o << "base: " << candidate->base_ip<<":"<<candidate->base_port << " ";
    }
    o << "type: " << candidate->type << " ";
    
    return o.str();
}


/** @brief Terminate current session. */
bool FstJingle::terminate()
{
    if (pipeline) {
        pipeline->setState(GST_STATE_NULL);
    }

    if (conference) {
        conference->removeAllSessions();
    }
    
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

bool FstJingle::isNull()
{
    return (pipeline->current_state() == GST_STATE_NULL);
}

/** @brief Return true if local media preconfiguration is complete. 
    That means conference element is paused or running, pipeline is ready
    or more. */
bool FstJingle::isPreconfigured()
{
    bool confstate = (GST_STATE(conference->conference()) >= GST_STATE_PAUSED);
    bool pipestate = (pipeline->current_state() >= GST_STATE_READY);
    return (confstate && pipestate && conference->codecsReady());
}

bool FstJingle::goPlaying()
{
    LOGGER(logit) << "setting to Playing" << std::endl;
    return pipeline->setState(GST_STATE_PLAYING);
}

bool FstJingle::goPaused()
{
    LOGGER(logit) << "setting to Paused" << std::endl;
    return pipeline->setState(GST_STATE_PAUSED);
}

bool FstJingle::goReady()
{
    LOGGER(logit) << "setting to Ready" << std::endl;
    return pipeline->setState(GST_STATE_READY);
}

bool FstJingle::goNull()
{
    LOGGER(logit) << "setting to Null" << std::endl;
    return pipeline->setState(GST_STATE_NULL);
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
    @param target JID or nickname of participant
    @return True if updated, false otherwise. 
*/
bool FstJingle::updateLocalTransport(JingleContent &content, const std::string &target)
{
    Stream *stream = conference->getStream(content.name(), target);
    if (stream) {
        GList * candidates = stream->getNewLocalCandidates();
        CandidateList cl = createJingleCandidateList(candidates);
        fs_candidate_list_destroy(candidates);
        JingleTransport jt = content.transport();
        jt.candidates = cl;
        content.setTransport(jt);
        return true;
    } else {
        LOGGER_WARN(logit) << "matching stream not found." << std::endl; 
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

/** @brief Return autodetected candidates for this host. 
    @param candidates GList of FsCandidate *
    @param anyip True to allow NULL ip in result, false to skip them. */
CandidateList   FstJingle::createJingleCandidateList(GList *candidates, bool anyip)
{
    CandidateList cl;
    while (candidates) {
        FsCandidate *c = static_cast<FsCandidate *>(candidates->data);
        if (c && (c->ip || anyip))
            cl.push_back(createJingleCandidate(c));
        candidates = g_list_next(candidates);
    }
    return cl;
}

/** @brief Try next candidate for this content. 
    @param content Content to update
    @param target JID or participant nickname
    @return true if there is next candidate to try, false otherwise.
    It will mark current candidate unreachable, and mark next candidate
    trying. If no more candidates, return false. 
*/
bool FstJingle::tryNextCandidate(JingleContent &content, const std::string &target)
{
    bool modified = false;
    std::vector<bool>   component_updated(5, false);
    bool next = false;

    JingleTransport jt = content.transport();
    for (CandidateList::iterator ci = jt.candidates.begin(); 
                ci!= jt.candidates.end(); ci++) 
    {
        next = (((int)component_updated.size()) > ci->component 
            && component_updated.at(ci->component));

        if (ci->reachable == JingleCandidate::REACHABLE_TRYING) {
            ci->reachable = JingleCandidate::REACHABLE_NO;
            LOGGER(logit) << "Candidate marked unreachable: " 
                << ci->ip << std::endl;
            modified = true;

        } else if (ci->reachable == JingleCandidate::REACHABLE_UNKNOWN 
                && !next) {
            if ((int) component_updated.size() > ci->component)
                component_updated.resize(ci->component+1, false);
            component_updated.assign(ci->component, true);

            ci->reachable = JingleCandidate::REACHABLE_TRYING;
            LOGGER(logit) << "Candidate marked trying: "
                << ci->ip << std::endl;
            modified = true;
            replaceRemoteCandidate(content.name(), *ci, target);
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


void FstJingle::setError(JingleErrors e, const std::string &msg)
{
    m_lastErrorCode = e;
    m_lastErrorMessage = msg;
    LOGGER(logit) << msg << " (" << msg << ")" << std::endl;
}

JingleErrors FstJingle::lastError()
{
    JingleErrors e = conference->lastError();
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
        LOGGER(logit) << "Unsupported transport namespace: " << xmlns<<std::endl;
        return std::string();
    }
}

void FstJingle::setTransportXmlns(const std::string &xmlns)
{
    std::string t = xmlnsToTransmitter(xmlns);
    if (!t.empty()) 
        conference->setTransmitter(t);
}

/** @brief Update list of available codecs to description. 
    @return true if content was updated from farsight, false if not. */
bool FstJingle::updateLocalDescription(JingleContent &content)
{
    FsMediaType ses_type = fsMediaType(content.description().type());
    Session *session = conference->getSession(ses_type);
    if (session) {
        if (!session->codecsReady()) {
            LOGGER(logit) << "Updating local contents " << content.name() 
                << " with codecs not ready!" << std::endl;
        }
        GList *codecs = session->getCodecListProperty("codecs");
        GList *i = codecs;
        JingleRtpContentDescription d = content.description();
        d.clearPayload();
        while (i) {
            d.addPayload(createJinglePayload((FsCodec *) i->data) );
            i = g_list_next(i);
        }
        content.setDescription(d);
        fs_codec_list_destroy(codecs);
        return true;
    } else {
        LOGGER(logit) << "Session of type " << ses_type 
                << " not found." << std::endl;
    }
    return false;
}

/** @brief Update list of available codecs to each local content description. 
    @return true if something was updated, false in other case. */
bool FstJingle::updateLocalDescription(JingleSession *session)
{
    ContentList cl = session->localContents();
    bool modified = false;
    for (ContentList::iterator it = cl.begin(); it!=cl.end(); it++) {
        if (updateLocalDescription(*it)) {
            modified = true;
        }
    }
    if (modified)
        session->replaceLocalContent(cl);
    return modified;
}

/** @brief Create Payload type from farsight codec. */
JingleRtpPayload FstJingle::createJinglePayload(const FsCodec *codec)
{
    JingleRtpPayload payload(codec->id, codec->encoding_name, 
        codec->clock_rate, codec->channels);
    const GList *parameters = codec->optional_params;
    while (parameters) {
        FsCodecParameter *p = (FsCodecParameter *) parameters->data;
        if (p) {
            payload.add(PayloadParameter(p->name, p->value));
        }
        parameters = g_list_next(parameters);
    }
    return payload;
}

PayloadList FstJingle::createJinglePayloadList(const GList *codecs)
{
    PayloadList pl;
    const GList *i = codecs;
    while (i) {
        JingleRtpPayload pay = createJinglePayload((FsCodec *) i->data);
        pl.push_back(pay);
    }
    return pl;
}

void FstJingle::reportFatalError(const std::string &msg)
{
    LOGGER(logit) << "FATAL ERROR: " << msg << std::endl;
    if (m_reader) {
        m_reader->reportMsg(FstStatusReader::MSG_FATAL_ERROR, msg);
        setState(S_FAILED);
        //m_reader->reportState(FstStatusReader::S_FAILED);
    }
}

void FstJingle::reportError(const std::string &msg)
{
    LOGGER(logit) << "ERROR: " << msg << std::endl;
    if (m_reader) {
        m_reader->reportMsg(FstStatusReader::MSG_ERROR, msg);
    }
}

void FstJingle::setMediaConfig(const MediaConfig &cfg)
{
    pipeline->setMediaConfig(cfg);
}

void FstJingle::setState(PipelineStateType s)
{
    m_state = s;
    if (m_reader) {
        m_reader->reportState(s);
    }
}

PipelineStateType FstJingle::state() const
{
    return m_state;
}

/** @brief Get list of supported resolutions of pad. */
FrameSizeList FstJingle::videoFrameSizes(GstPad *pad)
{
    FrameSizeList list;
    GstCaps *caps = NULL; // gst_pad_get_negotiated_caps(pad);
    if (!caps) {
        caps = gst_pad_get_caps(pad);
        if (!caps)
            return list;
    }
    for (unsigned int i = 0; i < gst_caps_get_size(caps); i++) {
        GstStructure *s = gst_caps_get_structure(caps, i);
        const GValue *h = gst_structure_get_value(s, "height");
        const GValue *w = gst_structure_get_value(s, "width");
        if (!h || !w) 
            continue;
        FrameSize size;
        if (G_VALUE_TYPE(h) == G_TYPE_INT) {
           size.height = g_value_get_int(h); 
           size.width = g_value_get_int(w);
           FrameSize::appendIfUnique(list, size);
        } else if (G_VALUE_TYPE(h) == G_TYPE_UINT) {
           size.height = g_value_get_uint(h); 
           size.width = g_value_get_uint(w);
           FrameSize::appendIfUnique(list, size);
        } else if (G_VALUE_TYPE(h) == GST_TYPE_INT_RANGE) {
            size.height = gst_value_get_int_range_min(h);
            size.width = gst_value_get_int_range_min(w);
            FrameSize::appendIfUnique(list, size);

            size.height = gst_value_get_int_range_max(h);
            size.width = gst_value_get_int_range_max(w);
            FrameSize::appendIfUnique(list, size);
        } else {
            LOGGER(logit) << "GValue for height has unsupported type " 
                << G_VALUE_TYPE(h) << std::endl;
        }
    } // for

    LOGGER(logit) << "Caps " << gst_caps_to_string(caps) 
        << " has sizes: " << FrameSize::toString(list) << std::endl;
    gst_caps_unref(caps);
    return list;
}

/** @brief Get list of supported frame sizes of video input. 
    It will support only fixed sizes. If device reports range of
    resolutions, i dont know what to do with it. */
FrameSizeList FstJingle::supportedVideoInputSizes()
{
    FrameSizeList list;
    GstElement *e = pipeline->getVideoSource();
    if (!e)
        return list;
    GstPad *pad = gst_element_get_pad(e, "src");
    if (!pad) {
        gst_object_unref(GST_OBJECT(e));
        return list;
    }
    list = videoFrameSizes(pad);
    gst_object_unref(GST_OBJECT(pad));
    gst_object_unref(GST_OBJECT(e));
    return list;
}

typedef std::list<std::string> StringList;

/** @brief Filters codec list and return it sorted by preferences, containing
    only enabled codecs. */
GList *FstJingle::codecListByPreference(const GList *codecs, const StringList &preferences)
{
    GList *newlist = NULL;

    for(StringList::const_iterator i=preferences.begin(); i!=preferences.end(); i++) {
        const GList *icodec = codecs;
        bool found = false;
        while (icodec != NULL) {
            FsCodec *codec = (FsCodec *) icodec->data;
            if (codec && (*i) == codec->encoding_name) {
                FsCodec *copy = fs_codec_copy(codec);
                newlist = g_list_prepend(newlist, copy);
                found = true;
            }
            icodec = g_list_next(icodec);
        }
    }
    return g_list_reverse(newlist);
}

/** @brief Convert jingle media type to farsight type. */
FsMediaType FstJingle::fsMediaType(MediaType type)
{
    switch (type) {
        case MEDIA_AUDIO:
            return FS_MEDIA_TYPE_AUDIO; 
        case MEDIA_VIDEO:
            return FS_MEDIA_TYPE_VIDEO; 
        case MEDIA_NONE:
        case MEDIA_AUDIOVIDEO:
            return FS_MEDIA_TYPE_AUDIO;
    }
    return FS_MEDIA_TYPE_AUDIO;
}

/** @brief Enable audio session and connect audio source to it.
    @return True if session was created already, or if were succesfully 
    created.
*/
bool FstJingle::enableAudioSession()
{
    bool enabled = pipeline->isAudioInputEnabled();
    if (!enabled) 
        enabled = pipeline->enableAudioInput();
    if (!enabled)
        return false;
    Session *session = conference->getSession(FS_MEDIA_TYPE_AUDIO);
    if (!session) {
        bool success = createSession(MEDIA_AUDIO, "audio");
        return success;
    } else {
        return true;
    }
}

/** @brief Enable video session and connect video source to it. 
    @return True if session was created already, or if it was succesfully 
    created. */
bool FstJingle::enableVideoSession()
{
    bool enabled = pipeline->isVideoInputEnabled();
    if (!enabled)
        enabled = pipeline->enableVideoInput();
    if (!enabled)
        return false;
    Session *session = conference->getSession(FS_MEDIA_TYPE_VIDEO);
    if (!session) {
        bool success = createSession(MEDIA_VIDEO, "video");
        return success;
    } else 
        return true;
}

