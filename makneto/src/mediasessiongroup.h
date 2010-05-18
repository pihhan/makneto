
#ifndef MEDIASESSIONGROUP_H
#define MEDIASESSIONGROUP_H

#include "sessionview.h"
#include "muccontrol.h"

class MediaManager;

/** @brief Glue makneto GUI classess with XMPP sessions and their 
video and audio outputs together.  */
class MediaSessionGroup : public JingleSessionGroup
{
    public:
    MediaSessionGroup(MediaManager *manager);
    
    void setMUC(MUC *muc);
    MUC *muc() const;

    SessionView * view() const;
    void setView(SessionView *view);

    MediaManager *manager() const;
    void setManager(MediaManager *manager);


    private:
    SessionView *m_view;
    MediaManager *m_manager;
    MUC         *m_muc;

};

#endif

