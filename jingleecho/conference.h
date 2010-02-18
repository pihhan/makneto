
#ifndef CONFERENCE_H
#define CONFERENCE_H

#include <string>
#include <list>

#include <gst/gst.h>
#include <gst/gstinterface.h>
#include <gst/farsight/fs-conference-iface.h>

#include "qpipeline.h"
#include "session.h"
#include "fststatusreader.h"

typedef enum {
    NoError = 0,
    AudioInputError,
    AudioOutputError,
    VideoInputError,
    VideoOutputError,
    NetworkError,
    NetworkTimeout,
    PipelineError,
    UnspecifiedError,
} JingleFarsightErrors;

typedef std::list<Session *>    SessionList;

/** Representation for one multimedia session between two people.
*/
class Conference
{
    public:

    Conference(GstElement *bin);
    Conference(QPipeline *pipeline);

    virtual ~Conference();

    GstElement * pipeline();
    GstElement * conference();

    FsParticipant * createParticipant( const std::string &name);

    void setInput(GstElement *input);
    void setOutput(GstElement *output);

    GList * getLocalCandidates();
    void addSession(Session *session);
    void removeSession(Session *session);
    void setNicknames(const std::string &local, const std::string &remote);

    FsParticipant * localParticipant()
    { return m_selfParticipant; }

    FsParticipant * remoteParticipant()
    { return m_remoteParticipant; }

    void onRecvCodecsChanged(GList *codecs);
    void onSendCodecsChanged(GList *codecs);
    void onLocalCandidatesPrepared();
    void onNewLocalCandidate(FsCandidate *candidate);

    static gboolean messageCallback(GstBus *bus, GstMessage *message, gpointer user_data);
    
    void srcPadAdded(Session *session, GstPad *pad, FsCodec *codec);

    std::string describe();

    Session *   getSession(const std::string &name);
    Session *   getSession(unsigned int id);
    bool        removeSession(const std::string &name);
    void        removeAllSessions();

    GList *     localCandidates();
    void        resetLocalCandidates();
    bool        haveLocalCandidates();

    void        resetNewLocalCandidates();
    bool        haveNewLocalCandidates();

    void        setStun(const std::string &ip, int port=0);
    std::string stunIp();
    int         stunPort();

    JingleFarsightErrors lastError();
    std::string         lastErrorMessage();
    void                setError(JingleFarsightErrors code, const std::string &message = std::string());

    void reportError(const std::string &msg);
    void reportFatalError(const std::string &msg);
    void reportWarning(const std::string &msg);
    void reportDebug(const std::string &msg);


    void setStatusReader(FstStatusReader *r);

    std::string transmitter() const;
    void setTransmitter(const std::string &t);

    private:
    void        increaseNewLocalCandidates();
    static std::string codecListToString(GList *codecs);

    QPipeline  * m_qpipeline;
    GstElement * m_pipeline;
    GstElement * m_fsconference;
    FsParticipant * m_selfParticipant;
    FsParticipant * m_remoteParticipant;
    SessionList     m_sessions;
    GList         * m_localCandidates;
    int         m_newLocalCandidates;
    std::string   m_stunIp;
    int           m_stunPort;
    JingleFarsightErrors m_lastErrorCode;
    std::string         m_lastErrorMessage;
    FstStatusReader     *m_reader;
    std::string         m_transmitter;
};

#endif

