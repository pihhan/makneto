
#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <gloox/gloox.h>
#include <gloox/logsink.h>

#define CL_DEBUG(client,message) (client)->logInstance().log(LogLevelDebug, \
    LogAreaUser, (message) )

namespace gloox {

/** @brief Class for logging output. For now, log into standard output. */
class Logger : public LogHandler
{
    public:
    Logger();

    virtual void handleLog(LogLevel level, LogArea area, const std::string &message);

    static std::string areaToString(LogArea area);
    static std::string levelToString(LogLevel level);
    static void init();

    void debug(const std::string &message);
    
};


} // namespace gloox

extern gloox::Logger * logger; 

#endif

