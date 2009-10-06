
#ifndef CONFEXCEPTION_H
#define CONFEXCEPTION_H

#include <exception>
#include <stdexcept>

class ConfException : public std::exception
{
    public:
    ConfException() throw();
    ConfException(const std::string &reason) throw();

    std::string reason() const;

    virtual const char *what() const throw();

    private:
    std::string m_reason;
};

#endif

