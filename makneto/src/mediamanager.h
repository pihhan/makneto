
#ifndef MEDIAMANAGER_H
#define MEDIAMANAGER_H

#include "makneto.h"
#include "jinglemanager.h"
#include "mediaconfig.h"

#include "fstjingle.h"
#include "maknetocontactlist.h"
#include "qtjinglemanager.h"
#include "fststatusreader.h"

/** @brief Connects media management and error reporting to GUI.
    @author Petr Mensik <pihhan@cipis.net> 

    This class is main manager of media sessions and their relations to 
    GUI. It interconnects gstreamer and farsight subsystem to signals and buttons
    on session widgets.
*/

class MediaManager : public QObject,
    public FstStatusReader
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

    void raiseIncomingCallPopup(const QString &caller, bool audio, bool video);

    /*
     * FstStatusReader functions. 
     */
    virtual void reportMsg(MessageType type, const std::string &comment);
    virtual void reportState(PipelineStateType state);
    virtual void contentStatusChanged(PipelineStateType state, const std::string &content, const std::string &participant = std::string());
    virtual void contentCandidatesActive(JingleCandidatePair &pair, const std::string &content);

    /* statusreader Qt signal shadow. */
    signals:
    void pipelineMessage(const QString &comment);
    void pipelineStateChange(PipelineStateType state);

    private:
    Makneto *m_makneto;
    QString m_ringFile;
    int     m_ringRepeated; ///! How many times ring played.
    int     m_ringDelay; ///! How long to wait after ring stops.
};



#endif


