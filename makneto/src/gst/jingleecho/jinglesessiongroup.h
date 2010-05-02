
#ifndef JINGLESESSIONGROUP_H
#define JINGLESESSIONGROUP_H

#include <string>
#include <list>
#include "jinglesession.h"

/**
    @brief Class for grouping several jingle sessions together.
    It is meant for multiparty jingle session call, ie. MUJI extension.
*/
class JingleSessionGroup
{
    public:
    typedef std::list<JingleSession *>  JingleSessionList;

    JingleSessionGroup();

    std::string     name() const;
    void setName(const std::string &name);

    void *data() const;
    void setData(void *data);

    void addSession(JingleSession *session);
    void removeSession(JingleSession *session);

    JingleSession * getSessionRemote(const std::string &remote) const;
    JingleSession * getSession(const PJid &remote) const;
    JingleSession * getSessionSid(const std::string &sid) const;

    protected: 
    std::string         m_name;
    JingleSessionList   m_sessions;
    void                *m_data;
};

#endif
