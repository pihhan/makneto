
#ifndef SESSION_H
#define SESSION_H

class Conference;

/** @brief Represents one media session and tools for working with that. */
class Session
{
    public:
    Session(Conference *conf);
    virtual ~Session();

    FsStream *createStream(FsParticipant *participant, const GList *lcandidates=NULL);
    /** @brief Create stream using local candidates already set 
        and remote participant of conference. */
    bool createStream();

    void setRemote(const std::string &ip, int port);
    void setRemote(GList *list);
    void setLocal(GList *candidates);
    void setLocalCodec(GList *codecs);
    bool setRemoteCodec(GList *codecs);
    GstPad * sink();

    static void srcPadAdded(FsStream *stream, GstPad *pad, FsCodec *codec, gpointer user_data);

    private:
    Conference *m_conf;
    FsSession *m_session;
    GError      *m_lasterror;
    FsStream    *m_stream;
    GList       *m_localCandidates;
};


#endif

