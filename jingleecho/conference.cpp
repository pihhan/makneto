
#include <iostream>
#include "conference.h"


Conference::Conference(GstElement *bin)
{
    m_pipeline = bin;
    GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE(bin));

    gst_bus_add_watch(bus, watch_bus, this);
    gst_object_unref(bus);

    m_fsconference = gst_element_factory_make("fsrtpconference", NULL);
    g_assert(m_fsconference);
    if (!gst_bin_add(GST_BIN(m_pipeline), m_fsconference)) {
        std::cerr << "Chyba pri pridavani conference do pipeline" << std::endl;
    }

}

FsParticipant * Conference::createParticipant( const std::string &name)
{
    return fs_conference_new_participant(
        FS_CONFERENCE(m_fsconference), name.c_str() );
}

GList * Conference::getLocalCandidates()
{
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
    
}
