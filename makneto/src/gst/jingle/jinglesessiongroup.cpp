
#include "jinglesessiongroup.h"

JingleSessionGroup::JingleSessionGroup()
    : m_data(0)
{
}

void JingleSessionGroup::addSession(JingleSession *session)
{
    m_sessions.push_back(session);
    session->setGroup(this);
}

void JingleSessionGroup::removeSession(JingleSession *session)
{
    JingleSessionList::iterator it;
    for (it = m_sessions.begin(); it != m_sessions.end(); it++) {
        if (*it == session)
            m_sessions.erase(it);
    }
    session->setGroup(NULL);
}

std::string JingleSessionGroup::name() const
{
    return m_name;
}

void JingleSessionGroup::setName(const std::string &name)
{
    m_name = name;
}

void * JingleSessionGroup::data() const
{
    return m_data;
}

void JingleSessionGroup::setData(void *data)
{
    m_data = data;
}

JingleSession * JingleSessionGroup::getSessionRemote(const std::string &remote) const
{
    JingleSessionList::const_iterator it;
    for (it=m_sessions.begin(); it!= m_sessions.end(); it++) {
        if (*it && (*it)->remote().fullStd() == remote)
            return *it;
    }
    return NULL;
}

JingleSession * JingleSessionGroup::getSession(const PJid &remote) const
{
    JingleSessionList::const_iterator it;
    for (it=m_sessions.begin(); it!= m_sessions.end(); it++) {
        if (*it && (*it)->remote() == remote)
            return *it;
    }
    return NULL;
}

JingleSession * JingleSessionGroup::getSessionSid(const std::string &sid) const
{
    JingleSessionList::const_iterator it;
    for (it=m_sessions.begin(); it!= m_sessions.end(); it++) {
        if (*it && (*it)->sid() == sid)
            return *it;
    }
    return NULL;
}

