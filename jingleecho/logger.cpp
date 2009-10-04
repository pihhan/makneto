
#include <iostream>
#include "logger.h"

using namespace gloox;

Logger *logger = NULL;



Logger::Logger()
{
    std::cout << "Logger created." << std::endl;
}

std::string Logger::areaToString(LogArea area)
{
    switch (area) {
        case LogAreaClassParser:
            return "parser";
        case LogAreaClassConnectionTCPBase:
            return "tcpbase";
        case LogAreaClassClient:
            return "client";
        case LogAreaClassClientbase:
            return "clientbase";
        case LogAreaClassComponent:
            return "component";
        case LogAreaClassDns:
            return "dns";
        case LogAreaClassConnectionHTTPProxy:
            return "http-proxy";
        case LogAreaClassConnectionSOCKS5Proxy:
            return "socks5-proxy";
        case LogAreaClassConnectionTCPClient:
            return "tcp-client";
        case LogAreaClassConnectionTCPServer:
            return "tcp-server";
        case LogAreaClassS5BManager:
            return "socks5-bytestream";
        case LogAreaXmlIncoming:
            return "incoming";
        case LogAreaXmlOutgoing:
            return "outgoing";
        case LogAreaUser:
            return "user-defined";
        default:
            return "unknown";
    }
}

std::string Logger::levelToString(LogLevel level)
{
    switch (level) {
        case LogLevelDebug:
            return "debug";
        case LogLevelWarning:
            return "warning";
        case LogLevelError:
            return "error";
        default:
            return "unknown";
    }
}

void Logger::handleLog(LogLevel level, LogArea area, const std::string &message)
{
    std::string prefix = "[" + levelToString(level) + "] ";

    prefix += "(" + areaToString(area) + ") ";

    std::cout << prefix << message << std::endl;
}

void Logger::init()
{
    logger = new Logger();
}

void Logger::debug(const std::string &message)
{
    handleLog(LogLevelDebug, LogAreaUser, message);
}
