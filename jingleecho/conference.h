
#ifndef CONFERENCE_H
#define CONFERENCE_H

#include <string>
#include <list>

#include <gst/gst.h>
#include <gst/gst_interface.h>
#include <gst/farsight/fs-conference-iface.h>

class Conference;

class Session
{
    public:
    Session(Conference *conf);

    FsStream *createStream(FsParticipant *participant);

    void setRemote(const std::string &ip, int port);
    void setRemote(const GList *list);
    void setLocalCodec(const GList *codecs);
    bool setRemoteCodec(const GList *codecs);
    GstPad * sink();

    private:
    FsSession *m_session;
    Conference *m_conf;
    GError      *m_lasterror;
    FsStream    *m_stream;
};

/** Representation for one multimedia session between two people.
*/
class Conference
{
    public:

    Conference(GstElement *bin);

    static bus_watch(GstBus *bus, GstElement *element, gpointer user_data);

    GstElement * pipeline();

    FsParticipant * createParticipant( const std::string &name);

    void setInput(GstElement *input);
    void setOutput(GstElement *output);

    GList * getLocalCandidates();

    private:
    GstElement * m_pipeline;
    GstElement * m_fsconference;
    FsParticipant * m_selfParticipant;
    FsParticipant * m_remoteParticipant;
};

#endif

