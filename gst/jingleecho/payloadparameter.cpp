
#include <sstream>
#include "payloadparameter.h"

/*
 *
 * PayloadParameter
 *
 */

PayloadParameter::PayloadParameter(const std::string &name, int value)
{
    m_type = TYPE_INT;
    m_name = name;
    ivalue = value;
    uivalue = ~0;
}

PayloadParameter::PayloadParameter(const std::string &name, unsigned int value)
{
    m_type = TYPE_UINT;
    m_name = name;
    uivalue = value;
    ivalue = -1;
}

PayloadParameter::PayloadParameter(const std::string &name, unsigned long value)
{
    m_type = TYPE_ULONG;
    m_name = name;
    ulvalue = value;
    ivalue = -1;
}

PayloadParameter::PayloadParameter(const std::string &name, const std::string &value)
{
    m_type = TYPE_STRING;
    m_name = name;
    svalue = value;
    uivalue = ~0;
    ivalue = -1;
}

int PayloadParameter::intValue() const
{
    switch (m_type) {
        case TYPE_INT:
        case TYPE_BOOL:
            return ivalue;
        case TYPE_UINT:
            return (int) uivalue;
        case TYPE_ULONG:
            return (int) ulvalue;
        case TYPE_STRING:
        {
            int x = -1;
            std::istringstream i(svalue);
            i >> x;
            return x; 
        }
    }
    return -1;
}

unsigned int PayloadParameter::uintValue() const
{
    switch (m_type) {
        case TYPE_UINT:
            return uivalue;
        case TYPE_BOOL:
        case TYPE_INT:
            return (unsigned int) ivalue;
        case TYPE_ULONG:
            return (unsigned int) ulvalue;
        case TYPE_STRING:
        {
            unsigned int x = -1;
            std::istringstream i(svalue);
            i >> x;
            return x; 
        }
    }
    return uivalue;
}

unsigned long PayloadParameter::ulongValue() const
{
    switch (m_type) {
        case TYPE_UINT:
            return uivalue;
        case TYPE_BOOL:
        case TYPE_INT:
            return ivalue;
        case TYPE_ULONG:
            return ulvalue;
        case TYPE_STRING:
        {
            unsigned long x = -1;
            std::istringstream i(svalue);
            i >> x;
            return x; 
        }
    }
    return ulvalue;
}

std::string PayloadParameter::stringValue() const
{
    std::ostringstream o;
    switch (m_type) {
        case TYPE_STRING:
            return svalue;
        case TYPE_UINT:
            o << uivalue; break;
        case TYPE_BOOL:
            if (ivalue)
                o << ivalue;
            else
                return "";
            break;
        case TYPE_INT:
            o << ivalue; break;
        case TYPE_ULONG:
            o << ulvalue; break;
    }
    return o.str();
}

PayloadParameter::ParamType PayloadParameter::type() const
{
    return m_type;
}

std::string PayloadParameter::name() const
{
    return m_name;
}


