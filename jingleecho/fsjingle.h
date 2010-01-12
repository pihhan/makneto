
#ifndef FSJINGLE_H
#define FSJINGLE_H

#include <gst/farsight.h>

#include "jinglesession.h"

/** @brief Class for converting my format for Jingle signalling to 
    farsight structures. */
class FstJingle
{
    public:
        
    static FsCandiate * createFsCandidate(const JingleCandidate & candidate);
    static FsCodec * createFsCodec(const JingleRtpPayload & payload);
    static GList * createFsCodecList(const JingleRtpContentDescription &description);
    static GList * createFsCandidateList(const JingleTransport &transport);
};

#endif
