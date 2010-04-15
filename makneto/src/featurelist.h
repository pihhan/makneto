/*!
 * \brief Capabilities and faetures management tools.
 *
 * @author Petr Mensik <pihhan@cipis.net>
 *
 * Simple classes for managing client's features and supported protocols.
 */

#ifndef FEATURELIST_H
#define FEATURELIST_H

#include "contactlist/status.h"

#include <QObject>
#include <QHash>
#include <QDomElement>
#include <QtAlgorithms>
#include <QStringList>
#include <xmpp_stanza.h>
#include <xmpp_discoitem.h>

#include "featurehelpers.h"
#include "discorequest.h"

class Makneto;

/*! \brief Class for managing capabilites of one client. */
class FeatureList : public QHash<QString, bool>
{
    public:
        FeatureList();

        /*! \brief Create Feature list from Disco#identity reply. */
        FeatureList(const QDomElement &query);

        /*! \brief Update this feature with new features from this reply. */
        void update(const QDomElement &query);

        /*! \brief Do we know features of this owner? 
            \return false if these features were not initialized from cache or disco reply.
            It means we do not know what client supports. */
        bool known() const
        {
            return m_known;
        }

        /*! \brief Is feature supported?
         * \param xmlns XML namespace of feature, as registered at http://xmpp.org/registrar/namespaces.html */
        bool supported(const QString & xmlns) const
        {
            return contains(xmlns);
        }

        void addFeature(const QString & feature)
        {
            insert(feature, true);
            m_known = true;
        }

        void addFeatures(const QStringList &features);

        /*! \brief Set prepared string to make security hash from. */
        void setHashable(const QString &hash)
        {
            m_hash = hash;
        }

        /*! \brief Return string object to be hashed as ver string .*/
        QString capsHashable() const
        {
            return m_hash;
        }

        /*! \brief Returns XML query element received as reply to disco#info request. */
        QDomElement query()
        {
            return m_query.documentElement();
        }

        static QString capsHashableStringList(const QStringList &list);

        /*! \brief Create part of XEP-155 verify string from forms included in list.
         * list is sorted in this method, it is modified then. */
        static QString capsHashableForms(XFormHelper::List &list);

        /*! \brief Create XEP-115 hash string from iq:query element.
         * \param query Child of iq stanza query, with xmlns of disco#info */
        static QString computeHashString(const QDomElement &query);
        static QString computeHashString(const XMPP::DiscoItem::Identity &identity, const XMPP::Features &features);

        void setNode(const QString &node)
        { m_node = node; }

        QString node() const
        { return m_node; }

        QString ver() const
        { return m_ver; }

        QString hash() const
        { return m_hash; }

        QString ext() const
        { return m_exthistory; }

        void setHash(const QString &algo, const QString &hash);

        void setVer(const QString &ver)
        { m_ver = ver; }

        void setCapsParams(const QString &node, const QString &ver, const QString &hash, const QString &ext);

        /*! \brief Parse list of extensions from ext attribute.
            \param extension list of extensions, delimited by spaces. */
        void parseExtensions(const QString &extension);

        bool hasExtension(const QString &extension);

        /*! \brief Returns list of extensions. */
        QStringList extensions()
        { return m_extensions; }

        bool hasFeaturesChanged(const QString &node, const QString &ver, const QString &hash=QString(), const QString &ext=QString());
        bool hasExtensionsChanged(const QString &ext);

        bool isLegacy() const;

        QString sha1Hash() const;

        QString computeHash(const QString &algo) const;

        QStringList allFeatures() const;

        QDomElement toXml(QDomDocument &doc) const;
        bool fromXml(QDomElement &element);


    private:

    bool m_known; ///<! Are features of this client known?
    QString     m_hash;
    QString     m_hashable;
    QDomDocument m_query;
    QStringList m_extensions; ///<! Legacy xep-115 extensions
    QString     m_exthistory; ///<! Extension parameter, used for comparison if ext changed since last parsing.
    QString     m_node;       ///<! XEP 115 node, something like client identifier
    QString     m_ver;
};




/*! \brief Simple database for capabilities of clients and mapping to its hashes. 
    \author Petr Mensik <xmensi06@stud.fit.vutbr.cz>
  */
class FeatureListManager : public QObject
{
    Q_OBJECT
    public:
        typedef QHash<QString, FeatureList *> ListOfFeatures;
        typedef QHash<QString, ListOfFeatures> HashedFeatures;
        typedef QHash<QString, FeaturesUpdateRequest *> RequestsHash;

        FeatureListManager(Makneto *makneto);

        FeatureList * getFeaturesByHash( const QString &ver, const QString &hash= QString());

        FeatureList * getFeatures( const QString &node, const QString &ver, const QString &hash=QString(), const QString &ext=QString());


        /* FIXME: tohle nebude fungovat spravne. nemam jeste pocitani skutecneho hashe. */
        void addFeatures(const FeatureList &fl);
        void addFeatures(FeatureList *fl);

        /*! \brief Reads features cache from xml file. */
        bool readFromFile(const QString &path);

        /*! \brief Create XML database structure for saving cache. */
        QDomDocument createXMLDatabase();

        bool writeToFile(const QString &path);

        /*! \brief Check if manager knows features of specified client. */
        bool isKnown(const QString &node, const QString &ver, const QString &hash, const QString & ext = QString());

        /*! \brief Request feature update using specified JID.
            It will send discovery requests to given JID, parse it results.
            If successful, it will fire featuresUpdated() signal. */
        void requestFeatureUpdate(XMPP::Client *client, const XMPP::Jid &jid, const QString &node, const QString &ver, const QString &hash, const QString & ext = QString());
        void requestFeatureUpdate(const XMPP::Jid &jid, const QString &node, const QString &ver, const QString &hash, const QString & ext = QString());

    signals:
        void featuresUpdated(XMPP::Jid jid, FeatureList *features);

    public slots:
        /*! \brief Connect here finished signal of feature update request. */
        void requestFinished(FeaturesUpdateRequest *ur);

        /*! \brief Write current known clients to database. */
        void writeDatabase();
        bool readDatabase();

        QString getDatabasePath();

        bool modified()
        { return m_modified; }
    
    protected:
        void removeRequest(FeaturesUpdateRequest *ur);
        FeaturesUpdateRequest *findRequest( const QString &node, const QString &ver, const QString &hash, const QString & ext = QString());
        QString singleCaps( const QString &node, const QString &ver, const QString &hash, const QString & ext = QString());

    private:

        HashedFeatures m_database; ///<! One database per one hash function.
        RequestsHash    m_requests;
        Makneto         *m_makneto;
        bool            m_modified;
        
};

#endif 

