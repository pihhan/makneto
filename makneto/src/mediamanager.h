
#ifndef MEDIAMANAGER_H
#define MEDIAMANAGER_H

#include <QTimer>

#include "fstjingle.h"
#include "jinglemanager.h"
#include "mediaconfig.h"

#include "irisjinglemanager.h"
#include "fststatusreader.h"
#include "qtaudioplayer.h"
#include "avparticipantcontrol.h"
#include "mediasessiongroup.h"

#include "makneto.h"
#include "maknetocontactlist.h"


typedef QList<AVParticipantControl *>   ParticipantControls;
/** @brief Connects media management and error reporting to GUI.
    @author Petr Mensik <pihhan@cipis.net> 

    This class is main manager of media sessions and their relations to 
    GUI. It interconnects gstreamer and farsight subsystem to signals and 
    buttons on session widgets.
*/
class MediaManager : public QObject,
    public FstStatusReader
{
    Q_OBJECT
    public:
    enum MediaMode { Unknown, AudioTest, VideoTest, Running };

    MediaManager(Makneto *makneto);

    /** @brief Parse media settings from KDE settings classes. */
    MediaConfig mediaSettings();

    /** @brief Begin playing specified file. */
    bool playRingFile(const QString &filename) ;
    /** @brief Replay current ring tone. */
    void replayRing();

    QString ringFile();
    
    FstJingle *jingle;
    IrisJingleManager *jingleManager;

    void setSessionGroup(MediaSessionGroup *sg);
    MediaSessionGroup *sessionGroup();

    bool audioInputReady();
    bool videoInputReady();

    void setMode(MediaMode mode);
    MediaMode mode() const;

    AVParticipantControl *createParticipant(const QString &jid);

    void testMediaSettings();
    void testAudioSettings();
    void testVideoSettings();

    bool configureDevice(
        MediaDevice &device,
        const QString &element,
        const QString &devname,
        const QString &parameters);

		
	QString locateSoundFile(const QString &filename) const;

    public slots:
    void startRingIncomingCall();
    void startRingIncomingVideoCall();
    void startRingIsRinging();
    void startRingBusy();
    void startRingTerminated();
    /** @brief Stop current ring file. */
    void stopRing();

    /** @brief Slot to start ring and ask user to accept or deny session. */
    void incomingSession(QtJingleSession *session);

    void mediaTestTimeout();
    void mediaTestComplete();
    
    void handleCallAccept();
    void handleCallDeny();

    QtAudioPlayer *player() const;

    /*
     * FstStatusReader functions. 
     */
    virtual void reportMsg(MessageType type, const std::string &comment);
    virtual void reportState(PipelineStateType state);
    virtual void contentStatusChanged(PipelineStateType state, const std::string &content, const std::string &participant = std::string());
    virtual void contentCandidatesActive(JingleCandidatePair &pair, const std::string &content);
    virtual void localCandidatesPrepared(const std::string &content, const std::string &participant);
    virtual void pipelineStateChanged(GstState state);

    /* statusreader Qt signal shadow. */
    signals:
    void pipelineMessage(const QString &comment);
    void pipelineStateChange(PipelineStateType state);

    /** @brief Signal emited when one call starts. */
    void mediaStarted();
    /** @brief Signal emited when call is ended and media pipeline is no longer
        needed. */
    void mediaEnded();
    void mediaTestFinished();
    void audioTestFinished();
    void videoTestFinished();
	
	void wentPaused();
	void wentPlaying();
	void wentStopped();
	void wentFailed();

    protected:
    void reportDebugMessage(const QString &message);

    private:
    Makneto *m_makneto;
    MediaSessionGroup *m_sessiongroup; ///! current session group if any.
    QString m_ringFile;
    int     m_ringRepeated; ///! How many times ring played.
    int     m_ringDelay; ///! How long to wait after ring stops.
    bool    m_videoInputReady;
    bool    m_audioInputReady;
    MediaMode    m_mode;
    ParticipantControls m_participants;
    QtAudioPlayer   *m_audioplayer;
    QTimer          m_mediaTestTimer;
    
    const static int  m_mediaTestTimerInterval = 3000; // ms
};



#endif


