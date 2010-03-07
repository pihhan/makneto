
#ifndef STREAM_H
#define STREAM_H

#include <glib.h>
#include <gst/gst.h>
#include <gst/farsight/fs-codec.h>
#include <gst/farsight/fs-participant.h>
#include <gst/farsight/fs-stream.h>
#include <gst/farsight/fs-conference-iface.h>

#include "fststatusreader.h"

class Conference;
class Session;

/** @brief Represents one media session and tools for working with that. */
class Stream
{
    public:
    Stream(Session *session, FsParticipant *participant, const GList *lcandidates = NULL);
    virtual ~Stream();

    FsStream *createStream(FsParticipant *participant, const GList *lcandidates);

    Session *session();

    void setRemote(const std::string &ip, int port);
    void setRemote(GList *list);
    void setLocal(GList *candidates);
    void setLocalPort(unsigned int port);
    void setLocalCodec(GList *codecs);
    bool setRemoteCodec(GList *codecs);

    void setDirection(FsStreamDirection d);
    FsStreamDirection direction() const;

    GList *getLocalCandidates();
    GList *getNewLocalCandidates();
    void clearLocalCandidates();

    FsCodec *currentSendCodec();
    GList   *currentRecvCodec();

    std::string describe();
    GList * getCodecListProperty(const char *name);

    bool        isSrcLinked();
    void        setSrcLinked(bool linked);

    void        onNewLocalCandidate(FsCandidate *candidate);

    FsSession   *sessionElement();
    FsStream    *streamElement();

    FsParticipant *participant() const;
    std::string participantName() const;

    std::string name() const;

    PipelineStateType   state() const;
    void        setState(PipelineStateType state);

    /* glib callbacks */
    static void srcPadAdded(FsStream *stream, GstPad *pad, FsCodec *codec, gpointer user_data);
    static void streamError(FsStream *self, FsError errno, gchar *error_msg,
    gchar *debug_msg, gpointer user_data);

    private:

    void resetError();

    Session     *m_confsession;
    GError      *m_lasterror;
    FsStream    *m_stream;
    GList       *m_localCandidates;
    GList       *m_newLocalCandidates;
    PipelineStateType   m_state;
};


#endif
