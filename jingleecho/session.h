
#ifndef SESSION_H
#define SESSION_H

#include <glib.h>
#include <gst/gst.h>
#include <gst/farsight/fs-codec.h>

class Conference;

/** @brief Represents one media session and tools for working with that. */
class Session
{
    public:
    Session(Conference *conf, FsMediaType type = FS_MEDIA_TYPE_AUDIO);
    virtual ~Session();

    FsStream *createStream(FsParticipant *participant, const GList *lcandidates);
    bool createStream(FsParticipant *participant);
    /** @brief Create stream using local candidates already set 
        and remote participant of conference. */
    bool createStream();
    bool haveStream();

    void setRemote(const std::string &ip, int port);
    void setRemote(GList *list);
    void setLocal(GList *candidates);
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
    FsStream    *m_stream;
    GList       *m_localCandidates;
    GList       *m_newLocalCandidates;
    std::string  m_name;
    bool         m_srclinked;
};


#endif

