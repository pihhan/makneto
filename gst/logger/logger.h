#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <fstream>
#include <string>

#define LOGGER(l)   ((l).at(__FILE__, __func__, __LINE__))

/** @brief Class for easier logging.
 *  @author Petr Mensik <pihhan@cipis.net>
 *
 *  Usage:
 *  Logger logf;
 *
 *  logf << "what you want to log" << std::endl;
 *  logf.printf("something special here: %s", "special"); // no endline needed
 *  LOGGER(logf) << "now we have it with file, line and function name" << std::endl;
 *  */
class Logger
{
    public:
    
    Logger();

    std::ostream & operator<<(std::ostream &o);
    std::ostream & operator<<(const char * const str);
    std::ostream & operator<<(int i);
    std::ostream & operator<<(double d);
    std::ostream & operator<<(const std::string &str);

    enum LogLevel {
        UNKNOWN = 0,
        DEBUG,
        INFO,
        WARNING,
        ERROR
    };


    std::ostream & stream();
    /** @brief Method to open logfile and output there.
 *      @param path File path to file.
 *      Until this is called, output is logged to stdout. 
 *      */
    bool open(const std::string &path);
    /** @brief Close and open again last file. */
    bool reopen();
    void close();
    /** @brief Specify file, fuction and line number only for next line */
    Logger & at(const char *file, const char *function, int line);
    /** @brief Print log using C function.
 *      @param fmt printf formatted string.
 *      */
    void printf(const char *fmt, ...);

    /** @brief Set log level for next line. */
    Logger & setLevel(LogLevel level);

    /** @brief Returns prefix that would be added to next line. */
    std::string prefix();

    private:
    void outputPlace(std::ostream &out);
    void resetPlace();
    void outputLevel(std::ostream &out);

    std::string    m_file;
    std::string    m_func;
    int             m_line;
    std::string    m_path;
    std::ofstream logfile;
    LogLevel        m_level;
};

extern Logger logit;

#define LOGGER_LVL(obj,level) (LOGGER(obj).setLevel(level))
#define LOGGER_DBG(l) (LOGGER_LVL(l,Logger::DEBUG))
#define LOGGER_INFO(l) (LOGGER_LVL(l,Logger::INFO))
#define LOGGER_WARN(l) (LOGGER_LVL(l,Logger::WARNING))
#define LOGGER_ERR(l) (LOGGER_LVL(l,Logger::ERROR))

#endif

