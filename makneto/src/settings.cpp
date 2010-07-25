// This file is generated by kconfig_compiler from makneto.kcfg.
// All changes you do to this file will be lost.

#include "settings.h"

#include <kglobal.h>
#include <QtCore/QFile>

class SettingsHelper
{
  public:
    SettingsHelper() : q(0) {}
    ~SettingsHelper() { delete q; }
    Settings *q;
};
K_GLOBAL_STATIC(SettingsHelper, s_globalSettings)
Settings *Settings::self()
{
  if (!s_globalSettings->q) {
    new Settings;
    s_globalSettings->q->readConfig();
  }

  return s_globalSettings->q;
}

Settings::Settings(  )
  : KConfigSkeleton( QLatin1String( "maknetorc" ) )
{
  Q_ASSERT(!s_globalSettings->q);
  s_globalSettings->q = this;
  setCurrentGroup( QLatin1String( "Account" ) );

  KConfigSkeleton::ItemString  *itemJabberID;
  itemJabberID = new KConfigSkeleton::ItemString( currentGroup(), QLatin1String( "jabberID" ), mJabberID );
  addItem( itemJabberID, QLatin1String( "jabberID" ) );
  KConfigSkeleton::ItemPassword  *itemJabberPassword;
  itemJabberPassword = new KConfigSkeleton::ItemPassword( currentGroup(), QLatin1String( "jabberPassword" ), mJabberPassword );
  addItem( itemJabberPassword, QLatin1String( "jabberPassword" ) );
  KConfigSkeleton::ItemBool  *itemStorePassword;
  itemStorePassword = new KConfigSkeleton::ItemBool( currentGroup(), QLatin1String( "storePassword" ), mStorePassword );
  addItem( itemStorePassword, QLatin1String( "storePassword" ) );
  KConfigSkeleton::ItemString  *itemJabberHost;
  itemJabberHost = new KConfigSkeleton::ItemString( currentGroup(), QLatin1String( "jabberHost" ), mJabberHost );
  addItem( itemJabberHost, QLatin1String( "jabberHost" ) );
  KConfigSkeleton::ItemInt  *itemJabberPort;
  itemJabberPort = new KConfigSkeleton::ItemInt( currentGroup(), QLatin1String( "jabberPort" ), mJabberPort, 5222 );
  addItem( itemJabberPort, QLatin1String( "jabberPort" ) );
  KConfigSkeleton::ItemString  *itemJabberFileTransferJID;
  itemJabberFileTransferJID = new KConfigSkeleton::ItemString( currentGroup(), QLatin1String( "jabberFileTransferJID" ), mJabberFileTransferJID );
  addItem( itemJabberFileTransferJID, QLatin1String( "jabberFileTransferJID" ) );
  KConfigSkeleton::ItemBool  *itemAllowPlain;
  itemAllowPlain = new KConfigSkeleton::ItemBool( currentGroup(), QLatin1String( "allowPlain" ), mAllowPlain, true );
  addItem( itemAllowPlain, QLatin1String( "allowPlain" ) );
  KConfigSkeleton::ItemBool  *itemUseSSL;
  itemUseSSL = new KConfigSkeleton::ItemBool( currentGroup(), QLatin1String( "useSSL" ), mUseSSL, false );
  addItem( itemUseSSL, QLatin1String( "useSSL" ) );
  KConfigSkeleton::ItemBool  *itemNoSSLWarn;
  itemNoSSLWarn = new KConfigSkeleton::ItemBool( currentGroup(), QLatin1String( "noSSLWarn" ), mNoSSLWarn, false );
  addItem( itemNoSSLWarn, QLatin1String( "noSSLWarn" ) );

  setCurrentGroup( QLatin1String( "Network" ) );

  KConfigSkeleton::ItemString  *itemStunServer;
  itemStunServer = new KConfigSkeleton::ItemString( currentGroup(), QLatin1String( "stunServer" ), mStunServer );
  addItem( itemStunServer, QLatin1String( "stunServer" ) );
  KConfigSkeleton::ItemString  *itemTurnServer;
  itemTurnServer = new KConfigSkeleton::ItemString( currentGroup(), QLatin1String( "turnServer" ), mTurnServer );
  addItem( itemTurnServer, QLatin1String( "turnServer" ) );
  KConfigSkeleton::ItemString  *itemTurnUsername;
  itemTurnUsername = new KConfigSkeleton::ItemString( currentGroup(), QLatin1String( "turnUsername" ), mTurnUsername );
  addItem( itemTurnUsername, QLatin1String( "turnUsername" ) );
  KConfigSkeleton::ItemPassword  *itemTurnPassword;
  itemTurnPassword = new KConfigSkeleton::ItemPassword( currentGroup(), QLatin1String( "turnPassword" ), mTurnPassword );
  addItem( itemTurnPassword, QLatin1String( "turnPassword" ) );

  setCurrentGroup( QLatin1String( "Media" ) );

  KConfigSkeleton::ItemString  *itemAudioInputModule;
  itemAudioInputModule = new KConfigSkeleton::ItemString( currentGroup(), QLatin1String( "AudioInputModule" ), mAudioInputModule );
  addItem( itemAudioInputModule, QLatin1String( "AudioInputModule" ) );
  KConfigSkeleton::ItemString  *itemAudioInputDevice;
  itemAudioInputDevice = new KConfigSkeleton::ItemString( currentGroup(), QLatin1String( "AudioInputDevice" ), mAudioInputDevice );
  addItem( itemAudioInputDevice, QLatin1String( "AudioInputDevice" ) );
  KConfigSkeleton::ItemString  *itemAudioInputParams;
  itemAudioInputParams = new KConfigSkeleton::ItemString( currentGroup(), QLatin1String( "AudioInputParams" ), mAudioInputParams );
  addItem( itemAudioInputParams, QLatin1String( "AudioInputParams" ) );
  KConfigSkeleton::ItemString  *itemAudioOutputModule;
  itemAudioOutputModule = new KConfigSkeleton::ItemString( currentGroup(), QLatin1String( "AudioOutputModule" ), mAudioOutputModule );
  addItem( itemAudioOutputModule, QLatin1String( "AudioOutputModule" ) );
  KConfigSkeleton::ItemString  *itemAudioOutputDevice;
  itemAudioOutputDevice = new KConfigSkeleton::ItemString( currentGroup(), QLatin1String( "AudioOutputDevice" ), mAudioOutputDevice );
  addItem( itemAudioOutputDevice, QLatin1String( "AudioOutputDevice" ) );
  KConfigSkeleton::ItemString  *itemAudioOutputParams;
  itemAudioOutputParams = new KConfigSkeleton::ItemString( currentGroup(), QLatin1String( "AudioOutputParams" ), mAudioOutputParams );
  addItem( itemAudioOutputParams, QLatin1String( "AudioOutputParams" ) );
  KConfigSkeleton::ItemString  *itemVideoInputModule;
  itemVideoInputModule = new KConfigSkeleton::ItemString( currentGroup(), QLatin1String( "VideoInputModule" ), mVideoInputModule );
  addItem( itemVideoInputModule, QLatin1String( "VideoInputModule" ) );
  KConfigSkeleton::ItemString  *itemVideoInputDevice;
  itemVideoInputDevice = new KConfigSkeleton::ItemString( currentGroup(), QLatin1String( "VideoInputDevice" ), mVideoInputDevice );
  addItem( itemVideoInputDevice, QLatin1String( "VideoInputDevice" ) );
  KConfigSkeleton::ItemString  *itemVideoInputParams;
  itemVideoInputParams = new KConfigSkeleton::ItemString( currentGroup(), QLatin1String( "VideoInputParams" ), mVideoInputParams );
  addItem( itemVideoInputParams, QLatin1String( "VideoInputParams" ) );
  KConfigSkeleton::ItemString  *itemVideoOutputModule;
  itemVideoOutputModule = new KConfigSkeleton::ItemString( currentGroup(), QLatin1String( "VideoOutputModule" ), mVideoOutputModule );
  addItem( itemVideoOutputModule, QLatin1String( "VideoOutputModule" ) );
  KConfigSkeleton::ItemString  *itemAudioRingModule;
  itemAudioRingModule = new KConfigSkeleton::ItemString( currentGroup(), QLatin1String( "AudioRingModule" ), mAudioRingModule );
  addItem( itemAudioRingModule, QLatin1String( "AudioRingModule" ) );
  KConfigSkeleton::ItemString  *itemAudioRingDevice;
  itemAudioRingDevice = new KConfigSkeleton::ItemString( currentGroup(), QLatin1String( "AudioRingDevice" ), mAudioRingDevice );
  addItem( itemAudioRingDevice, QLatin1String( "AudioRingDevice" ) );
  KConfigSkeleton::ItemBool  *itemAudioRingLoud;
  itemAudioRingLoud = new KConfigSkeleton::ItemBool( currentGroup(), QLatin1String( "AudioRingLoud" ), mAudioRingLoud );
  addItem( itemAudioRingLoud, QLatin1String( "AudioRingLoud" ) );
  KConfigSkeleton::ItemBool  *itemAudioRingPopup;
  itemAudioRingPopup = new KConfigSkeleton::ItemBool( currentGroup(), QLatin1String( "AudioRingPopup" ), mAudioRingPopup );
  addItem( itemAudioRingPopup, QLatin1String( "AudioRingPopup" ) );
  KConfigSkeleton::ItemString  *itemSoundIncoming;
  itemSoundIncoming = new KConfigSkeleton::ItemString( currentGroup(), QLatin1String( "soundIncoming" ), mSoundIncoming, QLatin1String( "ekiga/ring.wav" ) );
  addItem( itemSoundIncoming, QLatin1String( "soundIncoming" ) );
  KConfigSkeleton::ItemString  *itemSoundBusy;
  itemSoundBusy = new KConfigSkeleton::ItemString( currentGroup(), QLatin1String( "soundBusy" ), mSoundBusy, QLatin1String( "ekiga/busytone.wav" ) );
  addItem( itemSoundBusy, QLatin1String( "soundBusy" ) );
  KConfigSkeleton::ItemString  *itemSoundTerminated;
  itemSoundTerminated = new KConfigSkeleton::ItemString( currentGroup(), QLatin1String( "soundTerminated" ), mSoundTerminated );
  addItem( itemSoundTerminated, QLatin1String( "soundTerminated" ) );
  KConfigSkeleton::ItemString  *itemSoundRinging;
  itemSoundRinging = new KConfigSkeleton::ItemString( currentGroup(), QLatin1String( "soundRinging" ), mSoundRinging, QLatin1String( "ekiga/dialtone.wav" ) );
  addItem( itemSoundRinging, QLatin1String( "soundRinging" ) );
}

Settings::~Settings()
{
  if (!s_globalSettings.isDestroyed()) {
    s_globalSettings->q = 0;
  }
}

