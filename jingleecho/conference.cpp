
#include <iostream>
#include <sstream>

#include "conference.h"
#include "logger/logger.h"

#define LOGCF() (LOGGER(logit) << " conference " )

Conference::Conference(GstElement *bin)
    : m_qpipeline(0),m_selfParticipant(0),m_remoteParticipant(0)
{
    m_pipeline = bin;
    GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE(bin));

    gst_bus_add_watch(bus, messageCallback, this);
    gst_object_unref(bus);

    m_fsconference = gst_element_factory_make("fsrtpconference", NULL);
    g_assert(m_fsconference);
    if (!gst_bin_add(GST_BIN(m_pipeline), m_fsconference)) {
        LOGCF() << "Chyba pri pridavani conference do pipeline" << std::endl;
    }

}

Conference::Conference(QPipeline *pipeline)
    : m_qpipeline(pipeline),m_selfParticipant(0),m_remoteParticipant(0)
{
    m_pipeline = pipeline->element();
    GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE(m_pipeline));
    gst_bus_add_watch(bus, messageCallback, this);
    gst_object_unref(bus);

    m_fsconference = gst_element_factory_make("fsrtpconference", NULL);
    g_assert(m_fsconference);
    if (!gst_bin_add(GST_BIN(m_pipeline), m_fsconference)) {
        LOGGER(logit) << "Chyba pri pridavani conference do pipeline" << std::endl;
    }

}

FsParticipant * Conference::createParticipant( const std::string &name)
{
    GError *error = NULL;
    FsParticipant *p = fs_conference_new_participant(
        FS_CONFERENCE(m_fsconference), name.c_str(), &error );
    if (error) {
    LOGCF() << " fs_conference_new_participant: " <<
        error->message << std::endl;
    }
    return p;
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
    m_selfParticipant = createParticipant(local);
    m_remoteParticipant = createParticipant(remote);
}

void Conference::onNewLocalCandidate(FsCandidate *candidate)
{
    LOGCF() << "New candidate: " << candidate->ip << ":" << candidate->port 
        << std::endl;
}

void Conference::onLocalCandidatesPrepared()
{
    LOGCF() << "Local candidates prepared" << std::endl;
}

void Conference::onRecvCodecsChanged(GList *codecs)
{
    LOGCF() << "Recv codecs changed" << std::endl;
}

void Conference::onSendCodecsChanged(GList *codecs)
{
    LOGCF() << "Send codecs changed" << std::endl;
}

gboolean Conference::messageCallback(GstBus *bus, GstMessage *message, gpointer user_data)
{
    Conference *conf = (Conference *) user_data;

    switch (GST_MESSAGE_TYPE(message)) {
        case GST_MESSAGE_ERROR:
        case GST_MESSAGE_WARNING:
            {
                GError *error = NULL;
                gchar *debugmsg = NULL;
                gst_message_parse_error(message, &error, &debugmsg);
                LOGCF() << "Error: conference::messageCallback: " << debugmsg << std::endl;
            }
            break;
        case GST_MESSAGE_ELEMENT:
            {
                const GstStructure *s = gst_message_get_structure(message);
                LOGCF() << "Got element message with name: " 
                        <<  gst_structure_get_name(s) << std::endl;
                if (gst_structure_has_name(s, "farsight-error")) {
                    gint error;
                    const gchar *error_msg = gst_structure_get_string(s, "error-msg");
                    const gchar *debug_msg = gst_structure_get_string(s, "debug-msg");
                    g_assert(gst_structure_get_enum(s, "error-no", FS_TYPE_ERROR, &error));
                    if (FS_ERROR_IS_FATAL(error)) {
                        LOGGER(logit).printf("Farsight fatal error: %d %s %s",
                            error, error_msg, debug_msg);
                    } else {
                        LOGGER(logit).printf("Farsight non-fatal error: %d %s %s",
                            error, error_msg, debug_msg);
                    }

                } else if (gst_structure_has_name(s, 
                        "farsight-new-local-candidate")) {
                    const GValue *val = gst_structure_get_value(s, "candidate");
                    FsCandidate *candidate = NULL;
                    g_assert(val);
                    candidate = (FsCandidate *) g_value_get_boxed(val);
                    conf->onNewLocalCandidate(candidate);

                } else if (gst_structure_has_name(s, 
                        "farsight-local-candidates-prepared")) {
                    conf->onLocalCandidatesPrepared();

                } else if (gst_structure_has_name(s,
                        "farsight-recv-codecs-changed")) {
                    const GValue *v = gst_structure_get_value(s, "codecs");
                    GList *codecs = NULL;
                    g_assert(v);
                    codecs = (GList *) g_value_get_boxed(v);
                    conf->onRecvCodecsChanged(codecs);

                } else if (gst_structure_has_name(s,
                        "farsight-send-codecs-changed")) {
                    const GValue *v = gst_structure_get_value(s, "codecs");
                    GList *codecs = NULL;
                    g_assert(v);
                    codecs = (GList *) g_value_get_boxed(v);
                    conf->onSendCodecsChanged(codecs);
                } else {
                    const gchar *name = gst_structure_get_name(s);
                    std::cerr << "Neznama zprava na pipeline: " <<
                            name << std::endl;
                }
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

/** @brief Handle notification from remote party to connect pad somewhere. */
void Conference::srcPadAdded(Session *session, GstPad *pad, FsCodec *codec)
{
    LOGCF() << "Source pad added: " << gst_pad_get_name(pad) <<
            " with codec: " << fs_codec_to_string(codec) <<  std::endl;
    if (!m_qpipeline)
        return;
    GstPad *sinkpad = m_qpipeline->getAudioSinkPad();
    if (!gst_pad_is_linked(sinkpad)) {
        LOGCF() << "linkuji pad na audio sink" << std::endl;
        GstPadLinkReturn r = gst_pad_link(pad, sinkpad);
        if (GST_PAD_LINK_FAILED(r)) {
            LOGCF() << "Link zdroje na sink selhal!" << r << std::endl;
        }
    } else {
        LOGCF() << "sink je uz prilinkovan!" << std::endl;
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
    return o.str();
}

