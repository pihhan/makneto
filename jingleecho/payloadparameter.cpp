
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
    return ivalue;
}

unsigned int PayloadParameter::uintValue() const
{
    return uivalue;
}

std::string PayloadParameter::stringValue() const
{
    return svalue;
}

PayloadParameter::ParamType PayloadParameter::type() const
{
    return m_type;
}

std::string PayloadParameter::name() const
{
    return m_name;
}


