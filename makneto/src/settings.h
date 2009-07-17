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

  private:
};

#endif
