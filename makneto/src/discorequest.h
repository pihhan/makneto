
#ifndef DISCOREQUEST_H
#define DISCOREQUEST_H

#include <QObject>
#include <QString>
#include <QList>
#include <QDomDocument>

#include <xmpp_task.h>
#include <xmpp_jid.h>

#define XMLNS_DISCO_INFO ("http://jabber.org/protocol/disco#info")

class FeatureList;
class FeatureListManager;

/*! \brief Class to initiate disco#info request */
class DiscoRequest : public XMPP::Task
{
    Q_OBJECT

    public:

    /*! \brief Construct disco#info request to create new entry in manager. */
    DiscoRequest(XMPP::Task *parent, FeatureListManager *manager=NULL);
    /*! \brief Construct request to update existing features. */
    DiscoRequest(XMPP::Task *parent, FeatureList *features);

    void get(const QString &jid, const QString &node);
    virtual bool take(const QDomElement &element);

    void setFeatureManager(FeatureListManager *manager);

    QString node()
    { return m_node; }

    signals:
    void featuresUpdated();

    protected slots:
    void updateFeatureManager();

    protected:
    virtual void onGo();
    virtual void onDisconnect();

    private:
    QDomDocument    m_document; 
    QString         m_jid;
    QString         m_node;
    FeatureListManager *m_flm;
    FeatureList     *m_features;
    bool            m_delete_features;
    
};



/*! \brief Class for requesting all features of one JID.
    It sends as many disco#request is needed for extensions.
 */
class FeaturesUpdateRequest : public QObject
{
    Q_OBJECT

    typedef QList<XMPP::Jid>    JidList;
    public:
    FeaturesUpdateRequest(XMPP::Client *client, FeatureList *features, const QString &Jid);

    void setCapsParams(const QString &node, const QString &ver, const QString &hash, const QString &ext);

    void go(); 

    FeatureList * features()
    { return m_features; }

    QString jid() 
    { return m_jid.full(); }

    DiscoRequest * DiscoNodeRequest(const QString &subnode);

    void addJid(const XMPP::Jid &jid);

    public slots:
    void oneRequestFinished();

    signals:
    void finished(FeaturesUpdateRequest *ur);

    private:
    XMPP::Client    *m_client;
    FeatureList     *m_features;
    XMPP::Jid         m_jid;
    int             m_requestspending;
    int             m_errorcode;
    JidList         m_jids;

};

#endif

