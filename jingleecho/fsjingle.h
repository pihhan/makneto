
#ifndef FSJINGLE_H
#define FSJINGLE_H

#include <gst/gst.h>
#include <gst/gstinterface.h>
#include <gst/farsight/fs-conference-iface.h>
#include <gst/farsight/fs-conference-iface.h>

#include "jinglesession.h"
#include "qpipeline.h"
#include "conference.h"

/** @brief Class for converting my format for Jingle signalling to 
    farsight structures. */
class FstJingle
{
    public:
    FstJingle();
        
    static FsCandiate * createFsCandidate(const JingleCandidate & candidate);
    static FsCodec * createFsCodec(const JingleRtpPayload & payload);
    static GList * createFsCodecList(const JingleRtpContentDescription &description);
    static GList * createFsCandidateList(const JingleTransport &transport);

    QPipeline *pipeline;
    Conferene *conference;
};

#endif
