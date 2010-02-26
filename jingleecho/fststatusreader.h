
#ifndef FSTSTATUSREADER_H
#define FSTSTATUSREADER_H

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

    typedef enum {
        S_NONE = 0,
        S_PREPARING,
        S_RUNNING,
        S_STOPPED,
        S_FAILED
    } StateType;

    /** @brief Method to pass messages from farsight to GUI/log. 
        @param type Type of message, its importance.
        @param comment Text of message.*/
    virtual void reportMsg(MessageType type, const std::string &comment)=0;

    /** @brief Method to report state change to higher structures. */
    virtual void reportState(StateType state)=0;
};

#endif
