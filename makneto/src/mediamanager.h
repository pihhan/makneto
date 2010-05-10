
#ifndef MEDIAMANAGER_H
#define MEDIAMANAGER_H

#include "makneto.h"
#include "jinglemanager.h"
#include "mediaconfig.h"
#include "fstjingle.h"
#include "maknetocontactlist.h"
#include "qtjinglemanager.h"

/** @brief Connects media management and error reporting to GUI.
    @author Petr Mensik <pihhan@cipis.net> 
*/

class MediaManager : public QObject
{
    Q_OBJECT
    public:
    MediaManager(Makneto *makneto);

    /** @brief Parse media settings from KDE settings classes. */
    MediaConfig mediaSettings();

    /** @brief Begin playing specified file. */
    void playRingFile(const QString &filename) ;
    /** @brief Replay current ring tone. */
    void replayRing();
    /** @brief Stop current ring file. */
    void stopRing();

    QString ringFile();
    
    FstJingle *jingle;
    QtJingleManager *jingleManager;

    public slots:
    void startRingIncomingCall();
    void startRingIncomingVideoCall();
    void startRingIsRinging();
    void startRingBusy();
    void startRingTerminated();

    /** @brief Slot to start ring and ask user to accept or deny session. */
    void incomingSession(QtJingleSession *session);

    private:
    Makneto *m_makneto;
    QString m_ringFile;
    int     m_ringRepeated; ///! How many times ring played.
    int     m_ringDelay; ///! How long to wait after ring stops.
};



#endif


