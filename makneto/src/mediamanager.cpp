
#include "mediamanager.h"
#include "irisjinglemanager.h"
#include "settings.h"

MediaManager::MediaManager(Makneto *makneto)
    : m_makneto(makneto)
{
    jingle = new FstJingle();
}


/** @brief Parse media settings from KDE settings classes. */
MediaConfig MediaManager::mediaSettings()
{
    MediaConfig config;


    bool success = true;

    MediaDevice ain, aout;
    MediaDevice vin, vout;
    MediaDevice ring;
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
        Settings::videoOutputModule(), Settings::videoOutputDevice(), 
        Settings::videoOutputParams());

    success = success && configureDevice(ring, 
        Settings::audioRingModule(), Settings::audioRingDevice(), QString());

    if (success) {
        config.setAudioInput(ain);
        config.setAudioOutput(aout);
        config.setVideoInput(vin);
        config.setVideoOutput(vout);
        config.setLocalVideoOutput(vout);
        config.setRingOutput(ring);
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
    const QString &device,
    const QString &parameters)
{
    device.setElement(element.toStdString());
    device.setDevice(element.toStdString());

    std::string errorsmsg;
    bool parsed = device.parseStringParameters(parameters.toStdString(), errormsg);

    return parsed;
}

/** @brief Begin playing specified file. */
void MediaManager::playRingFile(const QString &filename) 
{
}

/** @brief Replay current ring tone. */
void MediaManager::replayRing()
{
}

/** @brief Stop current ring file. */
void MediaManager::stopRing()
{
}

QString MediaManager::ringFile()
{

}

FstJingle *jingle;
QtJingleManager *jingleManager;

void MediaManager::startRingIncomingCall()
{
}

void MediaManager::startRingIncomingVideoCall()
{
}

void MediaManager::startRingIsRinging()
{
}

void MediaManager::startRingBusy()
{
}

void MediaManager::startRingTerminated()
{
}

void MediaManager::incomingSession(QtJingleSession *session)
{

}

/*
 * FstStatusReader part.
 */

void MediaManager::reportMsg(MessageType type, const std::string &comment)
{
}

void MediaManager::reportState(PipelineStateType state)
{
}

void MediaManager::contentStatusChanged(PipelineStateType state, const std::string &content, const std::string &participant = std::string())
{
}

void MediaManager::contentCandidatesActive(JingleCandidatePair &pair, const std::string &content)
{
}

