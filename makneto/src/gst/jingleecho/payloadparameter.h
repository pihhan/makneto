
#ifndef PAYLOADPARAMETER_H
#define PAYLOADPARAMETER_H

#include <string>
#include <map>

/** @brief Parameter for class JingleRtpPayload. 
    @author Petr Mensik <pihhan@cipis.net>
    Allow basic typed storage. 
    It aims to be something like QVariant in Qt library. 
    */
class PayloadParameter {
    public:

    typedef enum {
        TYPE_INT,
        TYPE_UINT,
        TYPE_ULONG,
        TYPE_BOOL,
        TYPE_STRING
    } ParamType;

    PayloadParameter(const std::string &name, int value);
    PayloadParameter(const std::string &name, unsigned int value);
    PayloadParameter(const std::string &name, unsigned long value);
    PayloadParameter(const std::string &name, const std::string &value);

    int intValue() const;
    unsigned int uintValue() const;
    unsigned long ulongValue() const;
    std::string stringValue() const;
    ParamType type() const;
    std::string name() const;

    private:
    int ivalue;
    std::string m_name;
    unsigned int uivalue;
    unsigned long ulvalue;
    std::string svalue;
    ParamType   m_type;
};

typedef std::map<std::string, PayloadParameter> PayloadParameterMap;

#endif

