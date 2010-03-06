
#include <iostream>
#include <sstream>

#include <gst/farsight/fs-stream.h>
#include "conference.h"
#include "stream.h"
#include "fstjingle.h"

#include "logger/logger.h"

#define LOGCF() (LOGGER(logit) << " conference " )
#define DEFAULT_TRANSMITTER     "rawudp"

Conference::Conference(GstElement *bin)
    : m_qpipeline(0),m_selfParticipant(0),m_remoteParticipant(0),
      m_localCandidates(0), m_newLocalCandidates(0), m_stunPort(0), 
      m_lastErrorCode(NoError), m_reader(0), m_transmitter(DEFAULT_TRANSMITTER)
{
    m_pipeline = bin;
    gst_object_ref(m_pipeline);

    GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE(bin));

    gst_bus_add_watch(bus, messageCallback, this);
    gst_object_unref(bus);

    m_fsconference = gst_element_factory_make("fsrtpconference", NULL);
    if (!m_fsconference) {
        setError(PipelineError, "Farsight conference creation failed.");
        return;
    }
    if (!gst_bin_add(GST_BIN(m_pipeline), m_fsconference)) {
        LOGCF() << "Adding fsrtpconference to pipeline failed" << std::endl;
        setError(PipelineError, "Adding fsrtpconference to pipeline failed");
    }
}

Conference::Conference(QPipeline *pipeline)
    : m_qpipeline(pipeline),m_selfParticipant(0),m_remoteParticipant(0),
      m_localCandidates(0), m_newLocalCandidates(0), m_stunPort(0),
      m_lastErrorCode(NoError), m_reader(0), m_transmitter(DEFAULT_TRANSMITTER)
{
    m_pipeline = pipeline->element();
    GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE(m_pipeline));
    gst_bus_add_watch(bus, messageCallback, this);
    gst_object_unref(bus);

    m_fsconference = gst_element_factory_make("fsrtpconference", NULL);
    g_assert(m_fsconference);
    if (!gst_bin_add(GST_BIN(m_pipeline), m_fsconference)) {
        LOGGER(logit) << "Adding fsrtpconference to pipeline failed" 
                << std::endl;
        setError(PipelineError, "Adding fsrtpconference to pipeline failed");
    }

}

Conference::~Conference()
{
    removeAllSessions();

    //gst_object_unref(m_fsconference); // is unrefed with pipeline
    //gst_object_unref(m_pipeline);
}

/** @brief Create new participant with this name, or get existing from cache. */
FsParticipant * Conference::getParticipant(const std::string &name)
{
    ParticipantMap::iterator it = m_participants.find(name);
    if (it != m_participants.end()) {
        return it->second;
    } else {
        GError *error = NULL;
        FsParticipant *p = fs_conference_new_participant(
            FS_CONFERENCE(m_fsconference), name.c_str(), &error );
        if (error) {
        LOGCF() << "fs_conference_new_participant: " << name << ": " <<
            error->message << std::endl;
            setError(PipelineError, error->message);
        } else {
            m_participants.insert(make_pair(name, p));
        }

        return p;
    }
}

GList * Conference::getLocalCandidates()
{
#if 0
    GList *local_ips = fs_interfaces_get_local_ips(false);
    GList *start = g_list_first(local_ips);
    GList *candidates = NULL;

    const gchar *foundation = "foundation-test";
    const int component = 1;
    FsCandidateType ctype = FS_CANDIDATE_TYPE_HOST;
    FsNetworkProtocol ntype = FS_NETWORK_PROTOCOL_UDP;
    unsigned int port = 27945;
    if (start) {
        gchar *ip = g_list_next(local_ips);
        while (ip) {
            FsCandidate *candidate = fs_candidate_new(
                foundation,
                component,
                ctype, ntype,
                ip, port );
            g_list_prepend(candidates, candidate);
        }
    }
    return candidates;
#endif
    return NULL;
}

void Conference::addSession(Session *session)
{
    if (session)
        m_sessions.push_back(session);
}

void Conference::removeSession(Session *session)
{
    m_sessions.remove(session);
}

/** @brief Assign names to local and remote user, intended for JID. */
void Conference::setNicknames(const std::string &local, const std::string &remote)
{
    if (m_selfParticipant)
        g_object_unref(m_selfParticipant);
    if (m_remoteParticipant)
        g_object_unref(m_remoteParticipant);
    if (!m_selfParticipant)
        m_selfParticipant = getParticipant(local);
    if (!m_remoteParticipant)
        m_remoteParticipant = getParticipant(remote);
}

void Conference::onNewLocalCandidate(FsCandidate *candidate)
{
    LOGCF() << "New local candidate: " << candidate->ip << ":" 
        << candidate->port << std::endl;
    FsCandidate *copy = fs_candidate_copy(candidate);
    m_localCandidates = g_list_prepend(m_localCandidates, copy);
}

void Conference::onLocalCandidatesPrepared(GstMessage *message)
{
    FsStream *fsstream = NULL;
    if (!gst_structure_get(message->structure, 
        "stream", FS_TYPE_STREAM, &fsstream,
        NULL)) {
        LOGCF() << "gst_structure_get/stream failed" << std::endl;
        return;
    }
    Stream *my_stream = getStream(fsstream);
    if (my_stream) {
        if (my_stream->state() <= S_GATHERING) {
            my_stream->setState(S_GATHERED);
            LOGCF() << "Setting stream " << my_stream->participantName() 
                << " to gathered state." << std::endl;
        }
    }

    m_newLocalCandidates++;
    LOGCF() << "Local candidates prepared: " 
            << m_newLocalCandidates << std::endl;
}

void Conference::onRecvCodecsChanged(GList *codecs)
{
    LOGCF() << "Recv codecs changed" << codecListToString(codecs) << std::endl;
}

void Conference::onRecvCodecsChanged(GstMessage *message)
{
    FsStream *fss = NULL;
    GList *codecs = NULL;
    if (!gst_structure_get(message->structure,
        "codecs", FS_TYPE_CODEC_LIST, &codecs,
        "stream", FS_TYPE_STREAM, &fss,
        NULL)) {
        LOGCF() << "Could not get properties from recv-codecs-changed message" << std::endl;
        return;
    }
    onRecvCodecsChanged(codecs);
    gst_object_unref(GST_OBJECT(fss));
    fs_codec_list_destroy(codecs);
}

void Conference::onSendCodecsChanged(GList *codecs)
{
    LOGCF() << "Send codecs changed" << codecListToString(codecs) << std::endl;
}

/** @brief Handler for farsight message, emitted when state of stream is changed. */
void Conference::onComponentStateChanged(GstMessage *message)
{
    FsStream *fs = NULL;
    guint component = 0;
    FsStreamState state = FS_STREAM_STATE_FAILED;
    if (!gst_structure_get(message->structure,
        "stream", FS_TYPE_STREAM, &fs,
        "component", G_TYPE_UINT, &component,
        "state", G_TYPE_ENUM, &state,
        NULL)) {
        LOGCF() << "gst_structure_get failed" << std::endl;
    }
#if 0
    const GValue *vstate =gst_structure_get_value(message->structure, "state");
    FsStreamState state = (FsStreamState) g_value_get_enum(vstate);
#endif
    PipelineStateType pipestate = S_NONE;
    switch (state) {
        case FS_STREAM_STATE_FAILED:
            pipestate = S_FAILED; break;
        case FS_STREAM_STATE_DISCONNECTED:
            pipestate = S_DISCONNECTED; break;
        case FS_STREAM_STATE_GATHERING:
            pipestate = S_GATHERING; break;
        case FS_STREAM_STATE_CONNECTING:
            pipestate = S_CONNECTING; break;
        case FS_STREAM_STATE_CONNECTED:
            pipestate = S_CONNECTED; break;
        case FS_STREAM_STATE_READY:
            pipestate = S_ESTABLISHED; break;
    }

    Stream *stream = getStream(fs);
    if (stream && pipestate != S_NONE && component == FS_COMPONENT_RTP) {
        stream->setState(pipestate);
    }

    Session *session = getSession(fs);
    g_assert(stream && session);
    LOGGER(logit) << "Session " << session->name()
        << " stream " << stream->participantName()
        << " changed component " << component 
        << " state to " << state << std::endl;

    if (m_reader) {
        std::string name;
        if (session)
            name = session->name();
        gchar *participantname = NULL;
        g_object_get(G_OBJECT(fs), 
            "participant", &participantname, 
            NULL);

        PipelineStateType rstate = S_NONE;
        switch (state) {
            case FS_STREAM_STATE_FAILED:
                rstate = S_FAILED; break;
            case FS_STREAM_STATE_DISCONNECTED:
                rstate = S_DISCONNECTED; break;
            case FS_STREAM_STATE_GATHERING:
                rstate = S_GATHERING; break;
            case FS_STREAM_STATE_CONNECTING:
                rstate = S_CONNECTING; break;
            case FS_STREAM_STATE_CONNECTED:
                rstate = S_CONNECTED; break;
            case FS_STREAM_STATE_READY:
                rstate = S_ESTABLISHED; break;
        }

        m_reader->contentStatusChanged(rstate, name, participantname);
        g_free(participantname);
    }

}

/** @brief Handler of message emitted for new active candidate. */
void Conference::onNewActiveCandidate(GstMessage *message)
{
    FsCandidate *l = NULL;
    FsCandidate *r = NULL;
    FsStream *stream = NULL;
    if (!gst_structure_get(message->structure, 
            "stream", FS_TYPE_STREAM, &stream,
            "local-candidate", FS_TYPE_CANDIDATE, &l,
            "remote-candidate", FS_TYPE_CANDIDATE, &r,
            NULL)) {
        LOGCF() << "gst_structure_get for " 
            << gst_structure_get_name(message->structure) << " failed." 
            << std::endl;
        return ;
    }
    Stream *my_stream = getStream(stream);
    gst_object_unref(stream);
    Session *s = my_stream->session();
    if (s) {
        // TODO:
        //s->onNewActivePair(l, r);
        LOGCF() << "Session " << s->name() 
            << " stream " << my_stream->participantName()
            << " has new active pair "
            << l->ip << ":" << l->port
            << " <-> "
            << r->ip << ":" << r->port
            << " (only logged)"
            << std::endl;
    }

    JingleCandidatePair pair;
    pair.local = FstJingle::createJingleCandidate(l);
    pair.remote = FstJingle::createJingleCandidate(r);
    if (m_reader) {
        m_reader->contentCandidatesActive(pair, s->name());
    }
}

void Conference::onCodecsChanged(GstMessage *message)
{
    FsSession *fs = NULL;
    if (!gst_structure_get(message->structure,
            "session", FS_TYPE_SESSION, &fs,
            NULL)) {
        LOGCF() << "gst_structure_get failed for: " 
                << gst_structure_get_name(message->structure) << std::endl;
    } else {
        Session *session = getSession(fs);
        if (session) {
            GList *cl = session->getCodecListProperty("codecs");
            LOGCF() << "Session " << session->name() 
                << " changed codecs to " 
                << FstJingle::codecListToString(cl)
                << std::endl;
            fs_codec_list_destroy(cl);
        } else {
            LOGCF() << "session is NULL" << std::endl;
        }
    }
}

void Conference::onFarsightError(GstMessage *message)
{
    gint error;
    GstStructure *s = message->structure;
    const gchar *error_msg = gst_structure_get_string(s, "error-msg");
    const gchar *debug_msg = gst_structure_get_string(s, "debug-msg");
    g_assert(gst_structure_get_enum(s, "error-no", FS_TYPE_ERROR, &error));
    if (FS_ERROR_IS_FATAL(error)) {
        LOGGER(logit).printf("Farsight fatal error: %d %s %s",
            error, error_msg, debug_msg);
        reportFatalError(debug_msg);
    } else {
        LOGGER(logit).printf("Farsight non-fatal error: %d %s %s",
            error, error_msg, debug_msg);
        reportError(debug_msg);
    }
    setError(PipelineError, error_msg);
}

/** @brief Callback handle for element messages. */
gboolean Conference::elementMessageCallback(GstMessage *message)
{
        Conference *conf = this;
        const GstStructure *s = gst_message_get_structure(message);
        LOGCF() << "Got element message with name: " 
                <<  gst_structure_get_name(s) << std::endl;
        if (gst_structure_has_name(s, "farsight-error")) {
            onFarsightError(message);
        } else if (gst_structure_has_name(s, 
                "farsight-new-local-candidate")) {
            FsCandidate *candidate = NULL;
            FsStream *stream = NULL;
            if (!gst_structure_get(message->structure, 
                "stream", FS_TYPE_STREAM, &stream, 
                "candidate", FS_TYPE_CANDIDATE, &candidate,
                NULL)) {
                LOGCF() << "failed to get stream and candidate" << std::endl;
                return true;
            } else {
                unsigned int id = Session::idFromStream(stream);
                gst_object_unref(stream);

                Session *session = conf->getSession(id);
                if (session)
                    session->onNewLocalCandidate(candidate);
                conf->onNewLocalCandidate(candidate);
            }

        } else if (gst_structure_has_name(s, 
                "farsight-local-candidates-prepared")) {
            onLocalCandidatesPrepared(message);

        } else if (gst_structure_has_name(s,
                "farsight-new-active-candidate-pair")) {
            onNewActiveCandidate(message);

        } else if (gst_structure_has_name(s,
                "farsight-recv-codecs-changed")) {
            onRecvCodecsChanged(message);

        } else if (gst_structure_has_name(s,
                "farsight-send-codecs-changed") ) {
            const GValue *v = gst_structure_get_value(s, "codecs");
            GList *codecs = NULL;
            g_assert(v);
            codecs = (GList *) g_value_get_boxed(v);
            conf->onSendCodecsChanged(codecs);
        } else if (gst_structure_has_name(s, 
                "farsight-send-codec-changed")) {
            const GValue *v = gst_structure_get_value(s, "codec");
            GList *codecs = NULL;
            FsCodec *codec;
            g_assert(v);
            codec = (FsCodec *) g_value_get_boxed(v);
            codecs = g_list_prepend(codecs, fs_codec_copy(codec));
            conf->onSendCodecsChanged(codecs);
            fs_codec_list_destroy(codecs);

        } else if (gst_structure_has_name(s,
                "farsight-component-state-changed")) {
            onComponentStateChanged(message);
        } else if (gst_structure_has_name(s,
                "farsight-codecs-changed")) {
            onCodecsChanged(message);
        } else if (gst_structure_has_name(s,
                "prepare-xwindow-id")) {
            LOGCF() << "Received prepare-xwindow-id." << std::endl;
        } else {
            const gchar *name = gst_structure_get_name(s);
            gchar *srcname = gst_object_get_name(GST_OBJECT(message->src));
            LOGCF() << "Unknown element message from " 
                    << srcname << " on pipeline: " <<
                    name << std::endl;
            g_free(srcname);
        }
    return false;
}

/** @brief Message callback handling messages received from pipeline bus.
    @param bus Bus we received message from.
    @param message Message itself.
    @param user_data Pointer to this class.
*/
gboolean Conference::messageCallback(GstBus *bus, GstMessage *message, gpointer user_data)
{
    Conference *conf = (Conference *) user_data;

    switch (GST_MESSAGE_TYPE(message)) {
        case GST_MESSAGE_ERROR:
            {
                GError *error = NULL;
                gchar *debugmsg = NULL;
                gst_message_parse_error(message, &error, &debugmsg);
                LOGCF() << "Error: conference::messageCallback: " << debugmsg << std::endl;
                conf->setError(PipelineError, debugmsg);
                conf->reportError(debugmsg);
            }
            break;
        case GST_MESSAGE_WARNING:
            {
                GError *error = NULL;
                gchar *debugmsg = NULL;
                gst_message_parse_warning(message, &error, &debugmsg);
                LOGCF() << "Warning: conference::messageCallback: " << debugmsg << std::endl;
                conf->reportWarning(debugmsg);
                break;
            }
        case GST_MESSAGE_ELEMENT:
            return conf->elementMessageCallback(message);
        case GST_MESSAGE_NEW_CLOCK:
            {
                GstClock *clock = NULL;
                gchar *clock_name = NULL;
                gst_message_parse_new_clock(message, &clock);
                clock_name = gst_object_get_name(GST_OBJECT(clock));
                LOGCF() << "Installed new clock on pipeline: " 
                    << clock_name << std::endl;
                g_free(clock_name);
            }
            break;
        case GST_MESSAGE_STATE_CHANGED:
            {
                GstState olds, news, pending;
                gst_message_parse_state_changed(message, &olds, &news, &pending);
                char *name = gst_object_get_name(GST_OBJECT(message->src));
                LOGCF() << "Zmena stavu " << name << ": " << olds 
                        << " -> " << news 
                        << " (" << pending << ")" << std::endl;
                g_free(name);
            }
            break;
        case GST_MESSAGE_STREAM_STATUS:
            {
                GstElement *e = NULL;
                GstStreamStatusType type;
                gst_message_parse_stream_status(message, &type, &e);
                LOGCF() << "Stream status type " << type 
                        << " from element " << gst_element_get_name(e)
                        << std::endl;
            }
            break;
        default:
            LOGCF() << "Neoblouzeny typ zpravy " << gst_message_type_get_name(message->type) << std::endl;
            break;
            
    }
    return true;
}

GstElement * Conference::conference()
{
    return m_fsconference;
}

/** @brief Handle notification from remote party to connect pad somewhere. 
    This method is invoked inside gstreamer thread, not main thread! */
void Conference::srcPadAdded(Session *session, GstPad *pad, FsCodec *codec)
{
    LOGCF() << "Source pad added: " << gst_pad_get_name(pad) <<
            " with codec: " << fs_codec_to_string(codec) <<  std::endl;
    if (!m_qpipeline)
        return;
    GstPad *sinkpad = NULL;
    switch (session->type()) {
        case FS_MEDIA_TYPE_AUDIO:
            sinkpad = m_qpipeline->getAudioSinkPad();
            break;
        case FS_MEDIA_TYPE_VIDEO:
            sinkpad = m_qpipeline->getVideoSinkPad();
            break;

    }
    if (!sinkpad) {
        LOGCF() << "Src pad cannot be linked, sinkpad is missing!" << std::endl;
        return;
    }
    if (!gst_pad_is_linked(sinkpad)) {
        LOGCF() << "linking media " << session->name()
            << "source to sink..." << std::endl;
        GstPadLinkReturn r = gst_pad_link(pad, sinkpad);
        if (GST_PAD_LINK_FAILED(r)) {
            LOGCF() << "Link of source " << session->name() 
                << " to sink failed!" << r << std::endl;
            setError(AudioOutputError, "Link to sink failed.");
        } else {
            session->setSrcLinked(true);
        }
    } else {
        LOGCF() << "sink is already linked!" << std::endl;
    }
    GstPadTemplate *tmpl = gst_pad_get_pad_template(sinkpad);
    if (tmpl) {
        // pad with template is propably requested dynamic pad
        // FIXME: do something here?
    } else {
        // pad without is static
        gst_object_unref(sinkpad);
    }
}

/** @brief Create text description of this conference and its state. */
std::string Conference::describe()
{
    std::ostringstream o;
    o << "Conference: " << gst_element_get_name(m_fsconference) << std::endl;
    for (SessionList::iterator it=m_sessions.begin(); it!=m_sessions.end(); it++) {
        if (*it)
            o << (*it)->describe() << std::endl;
        else
            LOGCF() << "NULL Session!" << std::endl;
    }
    o << "Elements: " << QPipeline::binToString(m_fsconference);
    return o.str();
}

/** @brief Get sesion by content name. */
Session * Conference::getSession(const std::string &name)
{
    for (SessionList::iterator it=m_sessions.begin(); 
            it!=m_sessions.end(); it++) {
        if (*it && (*it)->name() == name)
            return *it;
    }
    return NULL;
}

/** @brief Get sesion by farsight session id. */
Session * Conference::getSession(unsigned int id)
{
    for (SessionList::iterator it=m_sessions.begin(); 
            it!=m_sessions.end(); it++) {
        if (*it && (*it)->id() == id)
            return *it;
    }
    return NULL;
}

/** @brief Get session by farsight session object. */ 
Session *   Conference::getSession(FsSession *fs)
{
    for (SessionList::iterator it=m_sessions.begin(); 
            it!=m_sessions.end(); it++) {
        FsSession *tmp = NULL;
        if (*it) {
            tmp = (*it)->sessionElement();
            if (tmp == fs) {
                gst_object_unref(tmp);
                return *it;
            }
            gst_object_unref(tmp);
        }
    }
    return NULL;
}

/** @brief Get session object by its stream. */
Session * Conference::getSession(FsStream *stream)
{
    FsSession *fs = NULL;
    g_object_get(G_OBJECT(stream), "session", &fs, NULL);
    if (fs) {
        Session *s = getSession(fs);
        gst_object_unref(GST_OBJECT(fs));
        return s;
    }
    return NULL;
}

/** @brief Get stream wrapper by farsight FsStream */
Stream  * Conference::getStream(FsStream *stream)
{
    Session *ses = getSession(stream);
    Stream *s = NULL;
    if (ses) {
       s = ses->getStream(stream);
    }
    return s;
}

/** @brief Get stream class by name of component(session) and name of participant(stream) */
Stream  * Conference::getStream(const std::string &component, const std::string &participant)
{
    Session *ses = getSession(component);
    Stream *s = NULL;
    if (ses) {
       s = ses->getStream(participant);
    }
    return s;
}


/** @brief remove and terminate one session with name. 
    @return true if session was terminated, false otherwise. */
bool Conference::removeSession(const std::string &name)
{
    Session *session = getSession(name);
    if (!session)
        return false;
    m_sessions.remove(session);
    delete session;
    return true;
}

void Conference::removeAllSessions()
{
    for (SessionList::iterator it=m_sessions.begin(); 
            it!=m_sessions.end(); it++) {
        if (*it) {
            delete (*it);
        }
    }
    m_sessions.clear();
}

GList * Conference::localCandidates()
{
    return fs_candidate_list_copy(m_localCandidates);
}

void Conference::resetLocalCandidates()
{
    if (m_localCandidates) {
        fs_candidate_list_destroy(m_localCandidates);
    }
    m_localCandidates = NULL;
}

bool Conference::haveLocalCandidates()
{
    return (m_localCandidates != NULL);
}
    
void Conference::resetNewLocalCandidates()
{
    m_newLocalCandidates = 0;
}

bool Conference::haveNewLocalCandidates()
{
    return (m_newLocalCandidates > 0);
}
    
void Conference::increaseNewLocalCandidates()
{
    m_newLocalCandidates++;
}

void Conference::setStun(const std::string &ip, int port)
{
    m_stunIp = ip;
    m_stunPort = port;
}

std::string Conference::stunIp()
{
    return m_stunIp;
}

int Conference::stunPort()
{
    return m_stunPort;
}

JingleErrors Conference::lastError()
{
    return m_lastErrorCode;
}

std::string         Conference::lastErrorMessage()
{
    return m_lastErrorMessage;
}

void Conference::setError(JingleErrors code, const std::string &message)
{
    m_lastErrorCode = code;
    m_lastErrorMessage = message;
}

void Conference::setStatusReader(FstStatusReader *r)
{
    m_reader = r;
}

void Conference::reportError(const std::string &msg)
{
    if (m_reader)
        m_reader->reportMsg(FstStatusReader::MSG_ERROR, msg);
}

void Conference::reportFatalError(const std::string &msg)
{
    if (m_reader)
        m_reader->reportMsg(FstStatusReader::MSG_FATAL_ERROR, msg);
}

void Conference::reportWarning(const std::string &msg)
{
    if (m_reader)
        m_reader->reportMsg(FstStatusReader::MSG_WARNING, msg);
}

void Conference::reportDebug(const std::string &msg)
{
    if (m_reader)
        m_reader->reportMsg(FstStatusReader::MSG_DEBUG, msg);
}

std::string Conference::codecListToString(GList *codecs)
{
    std::ostringstream o;
    GList *item = codecs;
    while (item) {
        gchar *str = fs_codec_to_string((FsCodec *)item->data);
        o << str << std::endl;
        g_free(str);
        item = g_list_next(item);
    }
    return o.str();
}

std::string Conference::transmitter() const
{
    return m_transmitter;
}

void Conference::setTransmitter(const std::string &t)
{
    m_transmitter = t;
}

/** @brief Check whether all sessions have codecs ready. 
    @return true if all session are ready and there is at least one, 
    false in other case. */
bool Conference::codecsReady() const
{
    bool ready = true;
    bool empty = true;
    for (SessionList::const_iterator it=m_sessions.begin(); 
        it!=m_sessions.end(); 
        it++) 
    {
        ready = ready && (*it)->codecsReady();
        empty = false;
        if (!ready) {
            LOGGER(logit) << "Session " << (*it)->name() 
                << " is not ready" << std::endl;
        }
    }
    return (ready && !empty);
}

