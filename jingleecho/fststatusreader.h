
#ifndef FSTSTATUSREADER_H
#define FSTSTATUSREADER_H

#include "jingle-candidate.h"
 
typedef enum {
    S_NONE = 0,
    S_PREPARING, ///<! preparing pipeline, elements and linking it
    S_GATHERING, ///<! gathering local candidates
    S_GATHERED,  ///<! local addresses are gathered
    S_CONNECTING, ///<! trying to connect
    S_CONNECTED, ///<! first response received, but fixed pairs
    S_ESTABLISHED, ///<! connection is acknowledged and confirmed by remote (ICE)
    S_DISCONNECTED, 
    S_RUNNING, ///!< Don't know what with this one
    S_STOPPED,
    S_FAILED
} PipelineStateType;

/** @brief Class to handle messages and status of conference pipeline.
    This should be implemented at endpoint, who will log messages and
    check status. */
class FstStatusReader
{
    public:
    typedef enum  {
        MSG_NONE = 0,
        MSG_DEBUG,
        MSG_INFO,
        MSG_WARNING,
        MSG_ERROR,
        MSG_FATAL_ERROR
    } MessageType;


    /** @brief Method to pass messages from farsight to GUI/log. 
        @param type Type of message, its importance.
        @param comment Text of message.*/
    virtual void reportMsg(MessageType type, const std::string &comment)=0;

    /** @brief Method to report state change to higher structures. */
    virtual void reportState(PipelineStateType state)=0;

    /** @brief Status of one stream changed. Report state change for content and participant names. */
    virtual void contentStatusChanged(PipelineStateType state, const std::string &content, const std::string &participant = std::string()) = 0;

    virtual void contentCandidatesActive(JingleCandidatePair &pair, const std::string &content) = 0;
};

#endif
