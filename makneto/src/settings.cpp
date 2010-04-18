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

  setCurrentGroup( QLatin1String( "Media" ) );

  QList<KConfigSkeleton::ItemEnum::Choice2> valuesaudioInputElement;
  KConfigSkeleton::ItemEnum  *itemAudioInputElement;
  itemAudioInputElement = new KConfigSkeleton::ItemEnum( currentGroup(), QLatin1String( "audioInputElement" ), mAudioInputElement, valuesaudioInputElement );
  addItem( itemAudioInputElement, QLatin1String( "audioInputElement" ) );
  QList<KConfigSkeleton::ItemEnum::Choice2> valuesaudioOutputElement;
  KConfigSkeleton::ItemEnum  *itemAudioOutputElement;
  itemAudioOutputElement = new KConfigSkeleton::ItemEnum( currentGroup(), QLatin1String( "audioOutputElement" ), mAudioOutputElement, valuesaudioOutputElement );
  addItem( itemAudioOutputElement, QLatin1String( "audioOutputElement" ) );
  QList<KConfigSkeleton::ItemEnum::Choice2> valuesvideoInputElement;
  KConfigSkeleton::ItemEnum  *itemVideoInputElement;
  itemVideoInputElement = new KConfigSkeleton::ItemEnum( currentGroup(), QLatin1String( "videoInputElement" ), mVideoInputElement, valuesvideoInputElement );
  addItem( itemVideoInputElement, QLatin1String( "videoInputElement" ) );
  QList<KConfigSkeleton::ItemEnum::Choice2> valuesvideoOutputElement;
  KConfigSkeleton::ItemEnum  *itemVideoOutputElement;
  itemVideoOutputElement = new KConfigSkeleton::ItemEnum( currentGroup(), QLatin1String( "videoOutputElement" ), mVideoOutputElement, valuesvideoOutputElement );
  addItem( itemVideoOutputElement, QLatin1String( "videoOutputElement" ) );
}

Settings::~Settings()
{
  if (!s_globalSettings.isDestroyed()) {
    s_globalSettings->q = 0;
  }
}

