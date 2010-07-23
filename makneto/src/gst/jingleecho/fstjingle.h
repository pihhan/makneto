
#ifndef FSJINGLE_H
#define FSJINGLE_H

#include <string>
#include <list>
#include <gst/gst.h>
#include <gst/gstinterface.h>
#include <gst/farsight/fs-conference-iface.h>
#include <gst/farsight/fs-candidate.h>
#include <gst/farsight/fs-codec.h>

#include "jinglesession.h"
#include "fststatusreader.h"
#include "mediaconfig.h"
#include "conference.h"
#include "qpipeline.h"
#include "framesize.h"


/** @brief Class for converting my format for Jingle signalling to 
    farsight structures. Direct farsight session. 
    @author Petr Mensik <pihhan@cipis.net> */
class FstJingle
{
    public:
    typedef std::list<std::string> StringList;

    FstJingle(FstStatusReader *reader = NULL);
    virtual ~FstJingle();
        
    void setNicknames(const std::string &local, const std::string &remote);
    bool linkSink(farsight::Session *session, FsMediaType type = FS_MEDIA_TYPE_AUDIO);
    bool prepareSession(const JingleContent &local);
    bool prepareSession(JingleSession *session);
    bool createAudioSession(const JingleContent &local, const JingleContent &remote);
    bool createAudioSession(JingleSession *session);
    bool updateRemote(const JingleContent &remote, const std::string &target = "");
    bool updateRemote(JingleSession *session);
    bool replaceRemoteContent(const JingleContent &content, const std::string &target);
    bool replaceRemoteCandidate(const std::string &content, const JingleCandidate &candidate, const std::string &target);
    std::string stateDescribe();
    bool terminate();


    /* Section of helpers to transform from jingle internal format to Farsight
        structures. */
    static FsCandidate * createFsCandidate(const JingleCandidate & candidate);
    static FsCodec * createFsCodec(const JingleRtpPayload & payload, FsMediaType type=FS_MEDIA_TYPE_AUDIO);
    static GList * createFsCodecList(const JingleRtpContentDescription &description);
    static GList * createFsCandidateList(const JingleTransport &transport);
    static GList * createSingleFsCandidateList(const JingleTransport &transport);
    static JingleCandidate createJingleCandidate(const FsCandidate *candidate);
    static JingleCandidate createJingleIceCandidate(const FsCandidate *candidate, const std::string &xmlns);
    static CandidateList   createJingleCandidateList(GList *candidates, bool anyip = true);
    static JingleRtpPayload createJinglePayload(const FsCodec *codec);
    static PayloadList createJinglePayloadList(const GList *codecs);

    static std::string codecListToString(const GList *codeclist);
    static std::string candidateListToString(const GList *codeclist);
    static std::string toString(const FsCodec *codec);
    static std::string toString(const FsCandidate *candidate);
    static std::string xmlnsToTransmitter(const std::string &xmlns);

    bool isPlaying();
    bool isPaused();
    bool isReady();
    bool isNull();
    bool isPreconfigured();

    bool goPlaying();
    bool goPaused();
    bool goReady();
    bool goNull();

    bool haveLocalCandidates();
    CandidateList   localCandidates();
    bool haveNewLocalCandidates();
    void resetNewLocalCandidates();
    
    bool updateLocalTransport(JingleContent &content, const std::string &target);
    bool tryNextCandidate(JingleContent &content, const std::string &target);

    bool updateLocalDescription(JingleContent &content);
    bool updateLocalDescription(JingleSession *session);

    void setStun(const std::string &ip, int port = 0);
    void setTransportXmlns(const std::string &xmlns);

    void setError(JingleErrors, const std::string &msg = std::string());
    JingleErrors lastError();
    std::string  lastErrorMessage();

    bool unconfiguredCodecs();

    void reportFatalError(const std::string &msg);
    void reportError(const std::string &msg);

    void setMediaConfig(const MediaConfig &cfg);

    PipelineStateType state() const;
    void setState(PipelineStateType s);

    static FrameSizeList videoFrameSizes(GstPad *pad);
    FrameSizeList supportedVideoInputSizes();

    bool createSession(MediaType type, const std::string &name);

    GList * codecListByPreference(const GList *codecs, const StringList &preferences);
    FsMediaType fsMediaType(MediaType type);

    bool enableAudioSession();
    bool enableVideoSession();

    QPipeline *pipeline;
    farsight::Conference *conference;

    private:
    JingleErrors m_lastErrorCode;
    std::string         m_lastErrorMessage;
    FstStatusReader     *m_reader;
    bool                m_unconfiguredCodecs;
    PipelineStateType   m_state;
};

#endif
