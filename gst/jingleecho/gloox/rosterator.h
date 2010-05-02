
#ifndef ROSTERATOR_H
#define ROSTERATOR_H

/**
 Handle roster information, status changes and stuff. 
 Do not forget log statuses :-)
 */

#include <gloox/stanza.h>
#include <gloox/clientbase.h>
#include <gloox/rostermanager.h>

namespace gloox {

class Rosterator : public RosterManager
{
    public:
        Rosterator(ClientBase *parent);

        virtual void handlePresence(Stanza *stanza);

    private:
        ClientBase *m_base;
};

}

#endif

