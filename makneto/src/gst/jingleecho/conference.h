
#ifndef CONFERENCE_H
#define CONFERENCE_H

#include <string>
#include <list>
#include <map>

#include <gst/gst.h>
#include <gst/gstinterface.h>
#include <gst/farsight/fs-conference-iface.h>

#include "qpipeline.h"
#include "session.h"
#include "fststatusreader.h"
#include "jingleerror.h"
#include "avoutput.h"
#include "avoutputmanager.h"


typedef std::list<Session *>    SessionList;
typedef std::map<std::string, FsParticipant *>  ParticipantMap;

/** @brief C++ wrap around FsConference and messages callback for directing
    farsight sessions.
    @author Petr Mensik <pihhan@cipis.net>
    
    Representation for one multimedia conference,
    with one or several sessions and each session having one or more stream.
    It is wrap around farsight FsConference, with some tools, and
    most important callback watcher for messages.
*/
class Conference
{
    public:

    Conference(GstElement *bin);
    Conference(QPipeline *pipeline);

    virtual ~Conference();

    GstElement * pipeline();
    GstElement * conference();

    FsParticipant * getParticipant( const std::string &name);

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
    void onRecvCodecsChanged(GstMessage *message);
    void onSendCodecsChanged(GList *codecs);
    void onLocalCandidatesPrepared(GstMessage *message);
    void onNewLocalCandidate(FsCandidate *candidate);
    void onNewLocalCandidate(GstMessage *message);

    void onComponentStateChanged(GstMessage *message);
    void onNewActiveCandidate(GstMessage *message);
    void onCodecsChanged(GstMessage *message);
    void onFarsightError(GstMessage *message);

    gboolean elementMessageCallback(GstMessage *message);
    static gboolean messageCallback(GstBus *bus, GstMessage *message, gpointer user_data);
    
    void srcPadAdded(Session *session, GstPad *pad, FsCodec *codec);

    std::string describe();

    Session *   getSession(const std::string &name);
    Session *   getSession(unsigned int id);
    Session *   getSession(FsSession *fs);
    Session *   getSession(FsStream *stream);
    Session *   getSessionType(FsMediaType type) const;
    Stream      *getStream(FsStream *stream);
    Stream      *getStream(const std::string &component, const std::string &participant);
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

    JingleErrors lastError();
    std::string         lastErrorMessage();
    void                setError(JingleErrors code, const std::string &message = std::string());

    void reportError(const std::string &msg);
    void reportFatalError(const std::string &msg);
    void reportWarning(const std::string &msg);
    void reportDebug(const std::string &msg);


    void setStatusReader(FstStatusReader *r);

    std::string transmitter() const;
    void setTransmitter(const std::string &t);

    bool codecsReady() const;
    PipelineStateType maxStreamState(const std::string &participant=std::string() );
    PipelineStateType minStreamState(const std::string &participant = std::string() );

    AVOutputManager & outputs() ;

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
    JingleErrors m_lastErrorCode;
    std::string         m_lastErrorMessage;
    FstStatusReader     *m_reader;
    std::string         m_transmitter;
    ParticipantMap      m_participants;
    AVOutputManager     m_outputs;
};

#endif

