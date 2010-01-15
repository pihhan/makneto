
#ifndef FSJINGLE_H
#define FSJINGLE_H

#include <string>
#include <gst/gst.h>
#include <gst/gstinterface.h>
#include <gst/farsight/fs-conference-iface.h>
#include <gst/farsight/fs-candidate.h>
#include <gst/farsight/fs-codec.h>

#include "jinglesession.h"
#include "conference.h"
#include "qpipeline.h"

/** @brief Class for converting my format for Jingle signalling to 
    farsight structures. Direct farsight session. */
class FstJingle
{
    public:
    FstJingle();
        
    static FsCandidate * createFsCandidate(const JingleCandidate & candidate);
    static FsCodec * createFsCodec(const JingleRtpPayload & payload);
    static GList * createFsCodecList(const JingleRtpContentDescription &description);
    static GList * createFsCandidateList(const JingleTransport &transport);
    static GList * createSingleFsCandidateList(const JingleTransport &transport);

    void setNicknames(const std::string &local, const std::string &remote);
    bool linkSink(Session *session);
    bool createAudioSession(const JingleContent &local, const JingleContent &remote);
    bool createAudioSession(JingleSession *session);
    std::string stateDescribe();

    static std::string codecListToString(const GList *codeclist);
    static std::string toString(const FsCodec *codec);

    QPipeline *pipeline;
    Conference *conference;
};

#endif
