// This file is generated by kconfig_compiler from makneto.kcfg.
// All changes you do to this file will be lost.
#ifndef SETTINGS_H
#define SETTINGS_H

#include <kconfigskeleton.h>
#include <kdebug.h>

class Settings : public KConfigSkeleton
{
  public:

    static Settings *self();
    ~Settings();

    /**
      Set Defines Jabber ID
    */
    static
    void setJabberID( const QString & v )
    {
      if (!self()->isImmutable( QString::fromLatin1 ( "jabberID" ) ))
        self()->mJabberID = v;
    }

    /**
      Get Defines Jabber ID
    */
    static
    QString jabberID()
    {
      return self()->mJabberID;
    }

    /**
      Set Defines Jabber password
    */
    static
    void setJabberPassword( const QString & v )
    {
      if (!self()->isImmutable( QString::fromLatin1 ( "jabberPassword" ) ))
        self()->mJabberPassword = v;
    }

    /**
      Get Defines Jabber password
    */
    static
    QString jabberPassword()
    {
      return self()->mJabberPassword;
    }

    /**
      Set Password storage
    */
    static
    void setStorePassword( bool v )
    {
      if (!self()->isImmutable( QString::fromLatin1 ( "storePassword" ) ))
        self()->mStorePassword = v;
    }

    /**
      Get Password storage
    */
    static
    bool storePassword()
    {
      return self()->mStorePassword;
    }

    /**
      Set Defines Jabber server host
    */
    static
    void setJabberHost( const QString & v )
    {
      if (!self()->isImmutable( QString::fromLatin1 ( "jabberHost" ) ))
        self()->mJabberHost = v;
    }

    /**
      Get Defines Jabber server host
    */
    static
    QString jabberHost()
    {
      return self()->mJabberHost;
    }

    /**
      Set Defines Jabber server port
    */
    static
    void setJabberPort( int v )
    {
      if (!self()->isImmutable( QString::fromLatin1 ( "jabberPort" ) ))
        self()->mJabberPort = v;
    }

    /**
      Get Defines Jabber server port
    */
    static
    int jabberPort()
    {
      return self()->mJabberPort;
    }


    /**
      Get Defines Jabber filetransfer proxy
    */
    static
    QString jabberFileTransferJID()
    {
      return self()->mJabberFileTransferJID;
    }

    /**
      Set Plaintext login
    */
    static
    void setAllowPlain( bool v )
    {
      if (!self()->isImmutable( QString::fromLatin1 ( "allowPlain" ) ))
        self()->mAllowPlain = v;
    }

    /**
      Get Plaintext login
    */
    static
    bool allowPlain()
    {
      return self()->mAllowPlain;
    }

    /**
      Set SSL encryption
    */
    static
    void setUseSSL( bool v )
    {
      if (!self()->isImmutable( QString::fromLatin1 ( "useSSL" ) ))
        self()->mUseSSL = v;
    }

    /**
      Get SSL encryption
    */
    static
    bool useSSL()
    {
      return self()->mUseSSL;
    }

    /**
      Set No SSL encryption warnings
    */
    static
    void setNoSSLWarn( bool v )
    {
      if (!self()->isImmutable( QString::fromLatin1 ( "noSSLWarn" ) ))
        self()->mNoSSLWarn = v;
    }

    /**
      Get No SSL encryption warnings
    */
    static
    bool noSSLWarn()
    {
      return self()->mNoSSLWarn;
    }


    /**
      Get STUN server
    */
    static
    QString stunServer()
    {
      return self()->mStunServer;
    }


    /**
      Get TURN server
    */
    static
    QString turnServer()
    {
      return self()->mTurnServer;
    }


    /**
      Get Username to use for TURN reservations
    */
    static
    QString turnUsername()
    {
      return self()->mTurnUsername;
    }


    /**
      Get Password to use for TURN reservations
    */
    static
    QString turnPassword()
    {
      return self()->mTurnPassword;
    }


    /**
      Get Name of GStreamer element to use as audio input
    */
    static
    int audioInputModule()
    {
      return self()->mAudioInputModule;
    }


    /**
      Get Parameter device of GStreamer audio input element.
    */
    static
    QString audioInputDevice()
    {
      return self()->mAudioInputDevice;
    }


    /**
      Get Space delimited additional options for audio input element.
    */
    static
    QString audioInputParams()
    {
      return self()->mAudioInputParams;
    }


    /**
      Get Gstreamer module to use
    */
    static
    int audioOutputModule()
    {
      return self()->mAudioOutputModule;
    }


    /**
      Get Parameter device for GStreamer module.
    */
    static
    QString audioOutputDevice()
    {
      return self()->mAudioOutputDevice;
    }


    /**
      Get Space delimited additional options for audio output element.
    */
    static
    QString audioOutputParams()
    {
      return self()->mAudioOutputParams;
    }


    /**
      Get VideoInputModule
    */
    static
    int videoInputModule()
    {
      return self()->mVideoInputModule;
    }


    /**
      Get Parameter device for GStreamer module.
    */
    static
    QString videoInputDevice()
    {
      return self()->mVideoInputDevice;
    }


    /**
      Get Space delimited additional options for video input element.
    */
    static
    QString videoInputParams()
    {
      return self()->mVideoInputParams;
    }


    /**
      Get Name of Gstreamer element to use as video output.
    */
    static
    int videoOutputModule()
    {
      return self()->mVideoOutputModule;
    }

  protected:
    Settings();
    friend class SettingsHelper;


    // Account
    QString mJabberID;
    QString mJabberPassword;
    bool mStorePassword;
    QString mJabberHost;
    int mJabberPort;
    QString mJabberFileTransferJID;
    bool mAllowPlain;
    bool mUseSSL;
    bool mNoSSLWarn;

    // Network
    QString mStunServer;
    QString mTurnServer;
    QString mTurnUsername;
    QString mTurnPassword;

    // Media
    int mAudioInputModule;
    QString mAudioInputDevice;
    QString mAudioInputParams;
    int mAudioOutputModule;
    QString mAudioOutputDevice;
    QString mAudioOutputParams;
    int mVideoInputModule;
    QString mVideoInputDevice;
    QString mVideoInputParams;
    int mVideoOutputModule;

  private:
};

#endif

