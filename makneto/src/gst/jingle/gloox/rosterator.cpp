
#include "rosterator.h"
#include "logger.h"

Rosterator::Rosterator(ClientBase *parent)
    : RosterManager(parent), m_base(parent)
{
    
}

void Rosterator::handlePresence(Stanza *stanza)
{

    switch (stanza->subtype()) {
        case StanzaPresenceUnavailable:
            CL_DEBUG(m_base, stanza->from() + "gone offline");
            break;
        case StanzaPresenceAvailable:
            CL_DEBUG(m_base, stanza->from() + "gone online");
            break;
        case StanzaPresenceProve:
            CL_DEBUG(m_base, stanza->from() + "is probing");
            break;
        case StanzaPresenceError:
            CL_DEBUG(m_base, stanza->from() + "got error presence");
            break;
        default:
            CL_DBUG(m_base, stanza->from() + " unhandled presence type !");
            break;
    }
    RosterManager::handlePresence(stanza);
}


