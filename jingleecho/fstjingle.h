
#ifndef FSJINGLE_H
#define FSJINGLE_H

#include <string>
#include <gst/gst.h>
#include <gst/gstinterface.h>
#include <gst/farsight/fs-conference-iface.h>
#include <gst/farsight/fs-candidate.h>
#include <gst/farsight/fs-codec.h>

#include "jinglesession.h"
#include "fststatusreader.h"
#include "conference.h"
#include "qpipeline.h"


/** @brief Class for converting my format for Jingle signalling to 
    farsight structures. Direct farsight session. */
class FstJingle
{
    public:
    FstJingle(JingleSession *js, FstStatusReader *reader = NULL);
    virtual ~FstJingle();
        
    void setNicknames(const std::string &local, const std::string &remote);
    bool linkSink(Session *session, FsMediaType type = FS_MEDIA_TYPE_AUDIO);
    bool prepareSession(const JingleContent &local);
    bool prepareSession(JingleSession *session);
    bool createAudioSession(const JingleContent &local, const JingleContent &remote);
    bool createAudioSession(JingleSession *session);
    bool updateRemote(const JingleContent &remote, const std::string &target = "");
    bool updateRemote(JingleSession *session);
    bool replaceRemoteContent(const JingleContent &content);
    bool replaceRemoteCandidate(const std::string &content, const JingleCandidate &candidate);
    std::string stateDescribe();
    bool terminate();

    static FsCandidate * createFsCandidate(const JingleCandidate & candidate);
    static FsCodec * createFsCodec(const JingleRtpPayload & payload);
    static GList * createFsCodecList(const JingleRtpContentDescription &description);
    static GList * createFsCandidateList(const JingleTransport &transport);
    static GList * createSingleFsCandidateList(const JingleTransport &transport);
    static JingleCandidate createJingleCandidate(const FsCandidate *candidate);
    static JingleCandidate createJingleIceCandidate(const FsCandidate *candidate, const std::string &xmlns);
    static CandidateList   createJingleCandidateList(GList *candidates, bool anyip = true);
    static JingleRtpPayload createJinglePayload(const FsCodec *codec);
    static PayloadList createJinglePayloadList(const GList *codecs);

    static std::string codecListToString(const GList *codeclist);
    static std::string toString(const FsCodec *codec);
    static std::string toString(const FsCandidate *candidate);
    static std::string xmlnsToTransmitter(const std::string &xmlns);

    bool isPlaying();
    bool isPaused();
    bool isReady();

    bool goPlaying();
    bool goPaused();
    bool goReady();

    bool haveLocalCandidates();
    CandidateList   localCandidates();
    bool haveNewLocalCandidates();
    void resetNewLocalCandidates();
    
    bool updateLocalTransport(JingleContent &content);
    bool tryNextCandidate(JingleContent &content);

    bool updateLocalDescription(JingleContent &content);
    bool updateLocalDescription(JingleSession *session);

    void setStun(const std::string &ip, int port = 0);
    void setTransportXmlns(const std::string &xmlns);

    void setError(JingleErrors, const std::string &msg = std::string());
    JingleErrors lastError();
    std::string  lastErrorMessage();

    bool unconfiguredCodecs();

    QPipeline *pipeline;
    Conference *conference;

    private:
    JingleErrors m_lastErrorCode;
    std::string         m_lastErrorMessage;
    FstStatusReader     *m_reader;
    bool                m_unconfiguredCodecs;
};

#endif
