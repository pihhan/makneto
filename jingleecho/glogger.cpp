
#include <iostream>
#include "glogger.h"
#include "logger/logger.h"

using namespace gloox;

GLogger *glogger = NULL;

GLogger::GLogger()
{
    logit << "Logger created." << std::endl;
}

std::string GLogger::areaToString(LogArea area)
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

std::string GLogger::levelToString(LogLevel level)
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

void GLogger::handleLog(LogLevel level, LogArea area, const std::string &message)
{
    std::string prefix = "[" + levelToString(level) + "] ";

    prefix += "(" + areaToString(area) + ") ";

    logit << prefix << message << std::endl;
}

void GLogger::init()
{
    glogger = new GLogger();
}

void GLogger::debug(const std::string &message)
{
    handleLog(LogLevelDebug, LogAreaUser, message);
}
