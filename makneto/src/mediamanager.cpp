
#include <QDebug>
#include <QTimer>
#include <kstandarddirs.h>
#include <typeinfo>

#include <kstatusbar.h>

#include "mediamanager.h"
#include "irisjinglemanager.h"
#include "settings.h"
#include "sessiontabmanager.h"
#include "qtaudioplayer.h"
#include "maknetoview.h"

#define SOUND_BASE_DIR  "/usr/share/sounds/ekiga"

MediaManager::MediaManager(Makneto *makneto)
    : m_makneto(makneto), m_sessiongroup(0), m_mode(Unknown)
{
    jingle = new FstJingle(this);

    jingleManager = 
        new IrisJingleManager(makneto->getConnection()->rootTask());
    connect(jingleManager, SIGNAL(sessionIncoming(QtJingleSession *)), 
            this, SLOT(incomingSession(QtJingleSession *)));

    m_audioplayer = new QtAudioPlayer();

    m_mediaTestTimer.setInterval(m_mediaTestTimerInterval);
    m_mediaTestTimer.setSingleShot(true);
    connect(&m_mediaTestTimer, SIGNAL(timeout()), 
        this, SLOT(mediaTestTimeout()));

    m_audioplayer->setMediaConfig(mediaSettings());

    // connect error messages to status bar
    //MaknetoMainWindow *mainwin = makneto->getMaknetoMainWindow();

    //connect(this, SIGNAL(pipelineMessage(QString)), 
    //        mainwin->statusBar(), SLOT(showMessage(QString)) );
}

/** @brief Begin test of audio and video subsystem settings.
    Check, if devices passed to configuration are valid. */
void MediaManager::testMediaSettings()
{
    connect(this, SIGNAL(audioTestFinished()),
        this, SLOT(testVideoSettings()) );
    connect(this, SIGNAL(videoTestFinished()),
        this, SLOT(mediaTestComplete()));

    testAudioSettings();
}

/** @brief Begin test of audio system. */
void MediaManager::testAudioSettings()
{
    MediaConfig config = mediaSettings();
    jingle->setMediaConfig(config);
    qDebug() << objectName() <<"beginning audio test...";
    m_mode = AudioTest;
    jingle->goPaused();
    m_mediaTestTimer.start();
}

/** @brief Begin video test. */
void MediaManager::testVideoSettings()
{
    MediaConfig config = mediaSettings();
    jingle->setMediaConfig(config);
    m_mode = VideoTest;
    qDebug() << objectName() << "beginning video test...";
    jingle->goPaused();
}

/** @brief Parse media settings from KDE settings classes into MediaConfig */
MediaConfig MediaManager::mediaSettings()
{
    MediaConfig config;

    bool success = true;

    MediaDevice ain = config.audioInput();
	MediaDevice aout = config.audioOutput();
    MediaDevice vin = config.videoInput();
	MediaDevice vout = config.videoOutput();
    MediaDevice ring = config.ringOutput();
	MediaDevice filein = config.fileInput();
	
    success = success && configureDevice(ain, 
        Settings::audioInputModule(), Settings::audioInputDevice(), 
        Settings::audioInputParams());
    success = success && configureDevice(aout, 
        Settings::audioOutputModule(), Settings::audioOutputDevice(), 
        Settings::audioOutputParams());
    success = success && configureDevice(vin,
        Settings::videoInputModule(), Settings::videoInputDevice(), 
        Settings::videoInputParams());
    success = success && configureDevice(vout,
        Settings::videoOutputModule(), "", 
        "");

    success = success && configureDevice(ring, 
        Settings::audioRingModule(), Settings::audioRingDevice(), QString());

    if (success) {
        config.setAudioInput(ain);
        config.setAudioOutput(aout);
        config.setVideoInput(vin);
        config.setVideoOutput(vout);
        config.setLocalVideoOutput(vout);
		//ring.setFilter("decodebin");
        config.setRingOutput(ring);
		filein = MediaDevice("filesrc");
		filein.setFilter("decodebin", MediaDevice::ELEMENT);
		config.setFileInput(filein);
    }
    return config;
}

/** @brief Parse configuration of one device from configuration settings. 
    @param device MediaDevice to configure from parameters passed
    @param element Element name
    @param device Name of device, format is dependend on element type.
    @param parameters Line with name=value pairs delimited by space. 
    @return True if device was successfuly configured, false if device 
    structure is left in unspecified state because of error. */
bool MediaManager::configureDevice(
    MediaDevice &device,
    const QString &element,
    const QString &devname,
    const QString &parameters)
{
    device.setElement(element.toStdString());
    if (!devname.isEmpty())
        device.addParameter(PayloadParameter("device", devname.toStdString()));

    std::string errorsmsg;
    bool parsed = 
        //device.parseStringParameters(parameters.toStdString(), errormsg);
        true;

    return parsed;
}

/** @brief Try to find sound file of specified name and return full path.
	@param filename Absolute or relative filename of sound.
	@return Full path to file if found, empty string otherwise.
*/
QString MediaManager::locateSoundFile(const QString &filename) const
{
    if (QDir::isAbsolutePath(filename)) {
        // play absolute path
        if (QFile(filename).exists()) {
            return (filename);
        } else 
            return QString();
    } else {
        KStandardDirs dirs;
        QString fullpath = KStandardDirs::locate("sound", filename);
        if (!fullpath.isEmpty()) {
            return fullpath;
        } else {
            return QString();
        }
    }
}

/** @brief Begin playing specified file. 
    @param filename Absolute or relative path to sound file.
    
    If filename is absolute path to file, it will be played.
    If it is relative path however, it will try to find file
    using KDE helpers in directories for sounds. */
bool MediaManager::playRingFile(const QString &filename) 
{
    MediaConfig config = mediaSettings();
    m_audioplayer->setMediaConfig(config);

    if (QDir::isAbsolutePath(filename)) {
        // play absolute path
        if (QFile(filename).exists()) {
            return m_audioplayer->playFile(filename);
        } else 
            qWarning() << "Sound file to be played does not exist: " << filename;
    } else {
        KStandardDirs dirs;
        QString fullpath = KStandardDirs::locate("sound", filename);
        if (!fullpath.isEmpty()) {
            return m_audioplayer->playFile(fullpath);
        } else {
            qWarning() <<"Sound file to be played was not found: " << filename;
        }
    }

#if 0
    QDir soundbase(SOUND_BASE_DIR);
    if (!soundbase.exists())
        return false;
    QString fullpath = soundbase.filePath(filename);
    if (QFile::exists(fullpath)) {
        return m_audioplayer->playFile(fullpath);
    }
#endif
    return false;
}

/** @brief Replay current ring tone. */
void MediaManager::replayRing()
{
    m_audioplayer->replay();
}

/** @brief Stop current ring file. */
void MediaManager::stopRing()
{
    m_audioplayer->stop();
}

QString MediaManager::ringFile()
{
    return locateSoundFile(Settings::soundIncoming());
}

void MediaManager::startRingIncomingCall()
{
	QString path = (locateSoundFile(Settings::soundIncoming()));
	m_audioplayer->setRepeats(-1);
	if (!path.isEmpty())
		playRingFile(path);
}

void MediaManager::startRingIncomingVideoCall()
{
	QString path = locateSoundFile(Settings::soundIncoming());
	m_audioplayer->setRepeats(-1);
	if (!path.isEmpty())
		playRingFile(path);
}

void MediaManager::startRingIsRinging()
{
	QString path = locateSoundFile(Settings::soundRinging());
	m_audioplayer->setRepeats(-1);
	if (!path.isEmpty())
		playRingFile(path);
}

void MediaManager::startRingBusy()
{
	QString path = locateSoundFile(Settings::soundBusy());
	m_audioplayer->setRepeats(1);
	if (!path.isEmpty())
		playRingFile(path);
}

void MediaManager::startRingTerminated()
{
	QString path = locateSoundFile(Settings::soundTerminated());
	m_audioplayer->setRepeats(1);
	if (!path.isEmpty())
		playRingFile(path);
}


/** @brief Handle incoming session, notify user about waiting call. 
    This will create AVParticipantControl for user, add that control
    to pending sessions list, and start ringing. 
    TODO: Check for existing session, reply with busy if there is
    existing session. */
void MediaManager::incomingSession(QtJingleSession *session)
{
    PJid remote = session->session()->remote();
    SessionTabManager *sessionmanager = m_makneto->getMaknetoMainWindow()
        ->getMaknetoView()->getSessionTabManager();
    SessionView *sv = sessionmanager->findSession(remote.full());
    if (sv) {
        // tab already exists.
        sessionmanager->bringToFront(sv);
    } else {
        // tab does not exist
        // FIXNE: this is not correct
        ChatType type = Chat;

        sv = sessionmanager->newSessionTab(remote.full(), type, remote.bare());
    }

    AVParticipantControl *control = createParticipant(remote.full());
    control->setSession(session);
    connect(control, SIGNAL(acceptedAudio()), this, SLOT(handleCallAccept()) );
    connect(control, SIGNAL(declined()), this, SLOT(handleCallDeny()) );
    m_participants.append(control);

    if (Settings::audioRingPopup())
        control->raisePopup();
    if (Settings::audioRingLoud())
        startRingIncomingCall();
}


/*
 * FstStatusReader part.
 */
void MediaManager::reportMsg(MessageType type, const std::string &comment)
{
    QString msg = QString::fromStdString(comment);
	QString prefix;
	switch (type) {
                case MSG_NONE:          prefix=""; break;
		case MSG_DEBUG:		prefix="DEBUG:"; break;
		case MSG_WARNING:	prefix="WARNING:"; break;
		case MSG_INFO:		prefix="INFO:"; break;
		case MSG_ERROR:		prefix="ERROR:"; break;
		case MSG_FATAL_ERROR: prefix="FATAL ERROR:"; break;
	}
    emit pipelineMessage(prefix + msg);
    reportDebugMessage(prefix + msg);
    qDebug() << "pipeline message type " << type << ": " << msg;
}

void MediaManager::reportState(PipelineStateType state)
{
    emit pipelineStateChange(state);
    qDebug() << "pipeline state change: " << state;
}

void MediaManager::contentStatusChanged(PipelineStateType state, const std::string &content, const std::string &participant)
{
    qDebug() << "contentStatusChanged, state " << state 
        << " content " << QString::fromStdString(content) 
        << " participant " << QString::fromStdString(participant);
}

void MediaManager::contentCandidatesActive(JingleCandidatePair &pair, const std::string &content)
{
    qDebug() << "contentCandidatesActive" <<
        QString::fromStdString(content);

    QString msg("Content %1 has new active pair: %2");
    reportDebugMessage(msg.arg(QString::fromStdString(content)).
        arg(QString::fromStdString(pair.toString()) ) );
}

void MediaManager::localCandidatesPrepared(
    const std::string &content, 
    const std::string &participant)
{
    qDebug() << "localCandidatesPrepared"
        << QString::fromStdString(content) << "@" 
        << QString::fromStdString(participant);
}


/** @brief Receive pipeline state changes from FstJingle. */
void MediaManager::pipelineStateChanged(GstState state)
{
    if (m_mode == VideoTest) {
        if (state == GST_STATE_PAUSED) {
            // test was successful
            m_videoInputReady = true;
            jingle->goNull();
            emit videoTestFinished();
        }
    } else if (m_mode == AudioTest) {
        if (state == GST_STATE_PAUSED) {
            jingle->goNull();
            m_audioInputReady = true;
            emit audioTestFinished();
        }
    } else if (m_mode == Running) {
    } else {
        qWarning() << "pipeline state changed in unknown state.";
    }
}

/** @brief Returns true if audio input subsytem is ready to use. */
bool MediaManager::audioInputReady()
{
    return m_audioInputReady;
}

/** @brief Returns true if video input subsystem is ready to use. */
bool MediaManager::videoInputReady()
{
    return m_videoInputReady;
}

void MediaManager::setMode(MediaMode mode)
{
    m_mode = mode;
}

MediaManager::MediaMode MediaManager::mode() const
{
    return m_mode;
}

/** @brief Create participant control object of given JID. */
AVParticipantControl * MediaManager::createParticipant(const QString &jid)
{
    AVParticipantControl *ctrl = new AVParticipantControl(this, jid);
    ChatType type = m_makneto->getContactList()->getContactType(jid);
    if (type == GroupChat) {
        XMPP::Jid roomjid(jid);
        User * usr = m_makneto->getMUCControl()->getUser(
                roomjid.bare(), roomjid.resource());
        ctrl->setMucUser(usr);
    } else if (type == Chat) {
        ctrl->setContact(m_makneto->getContactList()->getContact(jid));
    }
    return ctrl;
}


/** @brief Slot used for receiving accept signal from Participant Control. */
void MediaManager::handleCallAccept()
{
    AVParticipantControl *ctrl = dynamic_cast<AVParticipantControl *>(sender());
    stopRing();
    if (ctrl) {
        ctrl->session()->accept();
    }

    m_sessiongroup = new MediaSessionGroup(this);
}

/** @brief Slot used for deny signal from participant control. */
void MediaManager::handleCallDeny()
{
    AVParticipantControl *ctrl = dynamic_cast<AVParticipantControl *>(sender());
    stopRing();
    if (ctrl) {
        ctrl->session()->terminate();
    }

    if (Settings::audioRingLoud())
        startRingTerminated();
}

/** @brief This slot is triggered by timer, to cancel running media test
    availability. If it was not successful in time, mark tested media as
    not working. */ 
void MediaManager::mediaTestTimeout()
{
    if (m_mode == VideoTest) {
        m_videoInputReady = false;
        jingle->goNull();
        qDebug("Video test did timeout.");
        emit videoTestFinished();
    } else if (m_mode == AudioTest) {
        m_audioInputReady = false;
        jingle->goNull();
        qDebug("Audio test did tiemout.");
        emit audioTestFinished();
    } else {
        qDebug("MediaManager media test timeout when mode is not media test");
    }
}

/** @brief Media test complete. */
void MediaManager::mediaTestComplete()
{
    qDebug() << "Media test complete, status of audio: " << audioInputReady()
        << " video: " << videoInputReady();
}


QtAudioPlayer * MediaManager::player() const
{
    return m_audioplayer;
}

/** @brief Write debug message to main application status bar. */
void MediaManager::reportDebugMessage(const QString &message)
{
    MaknetoMainWindow *mainwin = m_makneto->getMaknetoMainWindow();
    mainwin->statusBar()->showMessage(message);
    qDebug() << objectName() << ": " << message;
}

void MediaManager::configureLocalPreviewWidget() 
{

}

