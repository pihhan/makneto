
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>

#include "logger.h"
#define DEFAULT_LOG_BLOCK_SIZE  4096

using namespace std;

Logger logit;

Logger::Logger()
    : m_line(-1)
{
}

ostream & Logger::operator<<(ostream &o)
{
    stream() << o;
    return (o);
}

ostream & Logger::operator<<(const char * const str)
{
    return stream() << prefix() << str;
}

ostream & Logger::operator<<(const std::string &str)
{
    return stream() << prefix() << str;
}

ostream & Logger::operator<<(int i)
{
    return stream() << prefix() << i;
}

ostream & Logger::operator<<(double d)
{
    return stream() << prefix() << d;
}

ostream & Logger::stream()
{
    if (!logfile)
        return cout;
    else 
        return cerr;
}

bool Logger::open(const std::string &path)
{
    m_path = path;
    logfile.open(path.c_str());
    return logfile.good();
}

bool Logger::reopen()
{
    close();
    return open(m_path);
}

void Logger::close()
{
    if (logfile.good()) 
        logfile.close();
    logfile.clear();
}

std::string Logger::prefix()
{
    time_t t;
    time(&t);
    char timestr[40];
    struct tm tstruct;
    localtime_r(&t, &tstruct);
    size_t s = strftime(timestr, 40, "%Y-%m-%dT%H:%M:%S", &tstruct);
    ostringstream os;
    if (s > 0 && s < 40) {
       os << "[" << timestr << "] "; 
    } else {
        os << "[" << t << "] ";
    }
    if (!m_file.empty()) {
        outputPlace(os);
        resetPlace();
    }
    return os.str();
}

void Logger::printf(const char *fmt, ...)
{
    int size = DEFAULT_LOG_BLOCK_SIZE;
    int retries = 100;
    char *buffer = NULL;
    va_list ap;

    buffer = (char *) malloc(size);
    if (!buffer)
        return;

    while (retries-- > 0) {
        int needed;
        va_start(ap, fmt);
        needed = vsnprintf(buffer, size, fmt, ap);
        if (needed > -1 && needed < size) {
            stream() << prefix() << buffer << endl;
            free(buffer);
            return;
        } else {
            char *newbuf;
            if (needed> -1) {
                size = needed+1;
            } else {
                size = size * 2;
            }
            newbuf = (char *) realloc(buffer, size);
            if (newbuf)
                buffer = newbuf;
            else return;
        }
    }
}

void Logger::outputPlace(ostream &out)
{
    out << m_file << ":" << m_line << " (" << m_func << "): ";
}

void Logger::resetPlace()
{
    m_file.clear();
    m_line = -1;
    m_func.clear();
}

Logger & Logger::at(const char *file, const char *function, int line)
{
    m_file = file;
    m_func = function;
    m_line = line;
    return *this;
}


