#include "confexception.h"


ConfException::ConfException()
{
}

ConfException::ConfException(const std::string &reason)
    : m_reason(reason)
{

}

const char * ConfException::what() 
{
    return m_reason.c_str();
}


