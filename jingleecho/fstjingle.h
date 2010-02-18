
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
    FstJingle(FstStatusReader *reader = NULL);
    virtual ~FstJingle();
        
    void setNicknames(const std::string &local, const std::string &remote);
    bool linkSink(Session *session);
    bool createAudioSession(const JingleContent &local, const JingleContent &remote);
    bool createAudioSession(JingleSession *session);
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
    static CandidateList   createJingleCandidateList(GList *candidates);

    static std::string codecListToString(const GList *codeclist);
    static std::string toString(const FsCodec *codec);
    static std::string xmlnsToTransmitter(const std::string &xmlns);

    bool isPlaying();
    bool isPaused();
    bool isReady();

    bool haveLocalCandidates();
    CandidateList   localCandidates();
    bool haveNewLocalCandidates();
    void resetNewLocalCandidates();
    
    bool updateLocalTransport(JingleContent &content);
    bool tryNextCandidate(JingleContent &content);

    void setStun(const std::string &ip, int port = 0);

    void setError(JingleFarsightErrors);
    JingleFarsightErrors lastError();
    std::string         lastErrorMessage();

    QPipeline *pipeline;
    Conference *conference;

    private:
    JingleFarsightErrors m_lastErrorCode;
    std::string         m_lastErrorMessage;
    FstStatusReader     *m_reader;
};

#endif
