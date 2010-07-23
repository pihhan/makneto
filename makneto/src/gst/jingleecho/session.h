
#ifndef SESSION_H
#define SESSION_H

#include <glib.h>
#include <gst/gst.h>
#include <gst/farsight/fs-codec.h>

#include <list>
#include <string>

namespace farsight {

class Conference;

class Stream;

typedef std::list<Stream *> StreamList;

/** @brief Represents one media session and basic api for woking with that.
    Session has its type, source pad for connection of media input to session,
    and list of created streams to each participant. */
class Session
{
    public:
    Session(Conference *conf, FsMediaType type = FS_MEDIA_TYPE_AUDIO);
    virtual ~Session();

    Conference *conference();

    Stream *createStream(FsParticipant *participant, const GList *lcandidates);
    bool createStream(FsParticipant *participant);
    /** @brief Create stream using local candidates already set 
        and remote participant of conference. */
    bool createStream();
    bool haveStream();

    Stream *firstStream();
    Stream *getStream(const std::string &participant);
    Stream *getStream(FsStream *stream);

    void setRemote(const std::string &ip, int port);
    void setRemote(GList *list);
    void setLocal(GList *candidates);
    void setLocalPort(unsigned int port);
    void setLocalCodec(GList *codecs);
    bool setRemoteCodec(GList *codecs);

    GstPad * sink();

    GList *getLocalCandidates();
    GList *getNewLocalCandidates();
    void clearLocalCandidates();

    FsCodec *currentSendCodec();
    GList   *currentRecvCodec();
    bool    codecsReady();
    unsigned int     rtpSsrc();
    int     rtcpTimeout();
    void    setRtcpTimeout(int timeout);

    std::string describe();
    GList * getCodecListProperty(const char *name);

    std::string name();
    void        setName(const std::string &name);
    bool        isSrcLinked();
    void        setSrcLinked(bool linked);

    void        onNewLocalCandidate(FsCandidate *candidate);

    FsSession   *sessionElement();
    FsStream    *streamElement();

    FsMediaType type() const;

    /** @brief Farsight id for this session. Used to lookup this class
        from @class Conference when only farsight structures are handled. */
    unsigned int    id();

    static      unsigned int idFromStream(FsStream *stream);
    static      unsigned int idFromStream(const GValue *val);

    StreamList      streams();
    int         streamCount();

    void streamDeleted();

    /* glib callbacks */
    static void srcPadAdded(FsStream *stream, GstPad *pad, FsCodec *codec, gpointer user_data);
    static void streamError(FsStream *self, FsError errno, gchar *error_msg,
    gchar *debug_msg, gpointer user_data);
    static void sessionError(FsSession *self, FsError errno, gchar *error_msg,
    gchar *debug_msg, gpointer user_data);

    private:

    void resetError();

    Conference *m_conf;
    FsSession *m_session;
    GError      *m_lasterror;
    GList       *m_localCandidates;
    GList       *m_newLocalCandidates;
    std::string  m_name;
    bool         m_srclinked;
    StreamList   m_streams;
};

}

#endif

