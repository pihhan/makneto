
#include <QHash>
#include <cstdio>
#include <QTextStream>
#include <QFile>
#include <QDir>

// hashing functions
#include <QtCrypto>

#include "featurelist.h"
#include "makneto.h"

#include <QDebug>


bool IdentityHelper::operator>(const IdentityHelper &other) const
{
    if (category == other.category) {
        if (type == other.type) {
            if (lang == other.lang) {
                return (name > other.name);
            } else {
                return (lang > other.lang);
            } // lang
        } else {
            return (type > other.type);
        } // type
    } else {
        return (category > other.category);
    } // category
}

/*! \brief Operator needed for sorting order according to XEP-115 */
bool IdentityHelper::operator<(const IdentityHelper &other) const
{
    if (category == other.category) {
        if (type == other.type) {
            if (lang == other.lang) {
                return (name < other.name);
            } else {
                return (lang < other.lang);
            } // lang
        } else {
            return (type < other.type);
        } // type
    } else {
        return (category < other.category);
    } // category
}

QStringList XFormFieldHelper::values()
{
    QStringList values;
    for (QDomElement child = m_element.firstChildElement("value");
        !child.isNull(); child = child.nextSiblingElement()) {
        if (!child.text().isEmpty())
            values.append(child.text());

    }
    return values;
}

/*! \brief Returns string for caps hashing for this field's values.
 * No fieldname is included. */
QString XFormFieldHelper::capsHashableValues()
{
    QString result;
    QStringList list = values();
    list.sort();
    for (QStringList::const_iterator it=list.begin(); it!=list.end();
        it++) {
        result.append( *it + "<" );
    }
    return result;
}

/*! \brief Returns string hashing for whole field, with values and keyname included. */
QString XFormFieldHelper::capsHashable()
{
    QString result = m_var + "<";
    result.append( capsHashableValues() );
    return result;
}

/*
 * XFormHelper
 */

QString XFormHelper::lookupFormType(const QDomElement &parent) const
{
    for (QDomElement e=parent.firstChildElement("field");
                !e.isNull(); e=e.nextSiblingElement("field")) {
        if (e.attribute("var")== "FORM_TYPE") {
            QDomElement value = e.firstChildElement("value");
            if (!value.isNull()) {
                return QString(value.text());
            }
        }
    }
    return QString();
}


void XFormHelper::loadFields(const QDomElement &parent) 
{
    for (QDomElement e=parent.firstChildElement("field");
                !e.isNull(); e=e.nextSiblingElement("field")) {
        m_fields.append(XFormFieldHelper(e));
    }

}

/*! \brief Returns part of string to hash for whole form. */
QString XFormHelper::capsHashable()
{
    QString hash(capsHashableFormType());
    sortFields();
    for (XFormFieldHelper::List::iterator it= m_fields.begin();
            it != m_fields.end(); it++) {
        hash.append( it->capsHashable() );
    }
    return hash;
}


/*
 * FeatureList
 */

FeatureList::FeatureList() 
    : m_known(true)
{
}

/*! \brief Construct feature list from reply of disco#info request. */
FeatureList::FeatureList(const QDomElement &query)
    : m_known(true)
{
    if (m_query.hasChildNodes())
        m_query.clear();

    QDomNode queryelem = m_query.importNode(query, true);
    m_query.appendChild(queryelem);

    m_hash = computeHashString(query);
    for (QDomElement e=query.firstChildElement("feature"); !e.isNull();
            e=e.nextSiblingElement("feature")) {
        addFeature(e.attribute("var"));
    }
}

/*! \brief Update feature list with another set o features.
    Useful for adding features in extensions. */
void FeatureList::update(const QDomElement &query)
{
    for (QDomElement e=query.firstChildElement("feature"); !e.isNull();
            e=e.nextSiblingElement("feature")) {
        addFeature(e.attribute("var"));
        m_known = true;
    }
    if (!m_query.hasChildNodes()) {
        QDomNode copy = m_query.importNode(query,true);
        m_query.appendChild(copy);
    }

}

void FeatureList::addFeatures(const QStringList &features)
{
    QStringList::const_iterator it;
    for (it= features.begin(); it!=features.end(); it++) {
        addFeature(*it);
    }
    m_known = true;
}

QString FeatureList::capsHashableStringList(const QStringList &list)
{
    QStringList list2(list);
    QString hashable;

    list2.sort();
    for (QStringList::const_iterator it=list2.begin(); 
            it!=list2.end(); it++) {
        hashable += (*it) + "<";
    }
    return hashable;
}

/*! \brief Create part of XEP-155 verify string from forms included in list.
 * list is sorted in this method, it is modified then. */
QString FeatureList::capsHashableForms(XFormHelper::List &list)
{
    QString hashable;
    qSort(list.begin(), list.end());
    for (XFormHelper::List::iterator it = list.begin();
            it != list.end(); it++) {
        hashable += it->capsHashable();
    }
    return hashable;
}


/*! \brief Create XEP-115 hash string from iq:query element.
 * \param query Child of iq stanza query, with xmlns of disco#info */
QString FeatureList::computeHashString(const QDomElement &query)
{
    QString appended;
    QString hashable; 
    QStringList features;
    QDomElement identity;
    IdentitySorter identsort;
    XFormHelper::List forms;

    for (QDomElement child = query.firstChildElement();
            !child.isNull(); child = child.nextSiblingElement()) {
        if (child.tagName() == "identity") {
            identsort.add(IdentityHelper(child));
            qDebug() << "capshash identity ";
        } else if (child.tagName() == "feature") {
            QString sf = child.attribute("var");
            qDebug() << "capshash feature " << sf;
            features.append(sf);
        } else if (child.attribute("xmlns") == "jabber:x:data") {
            // formulář s extended info
            forms.append(XFormHelper(child));
            qDebug() << "capshash form " ;
        } // do not try to understand other childen, we dont need them now
    }

    hashable = identsort.capsHashable();
    hashable += capsHashableStringList(features);
    hashable += capsHashableForms(forms);
    return hashable; 
}

/*! \brief Create list of extensions from one string delimited by spaces. */
void FeatureList::parseExtensions(const QString &extension)
{
    m_exthistory = extension;
    m_extensions = extension.split(" ", QString::SkipEmptyParts);
}

/*! \brief Returns if this feature class contains features from extension.
    \param extension Name of extensions. */
bool FeatureList::hasExtension(const QString &extension)
{
    return m_extensions.contains(extension);
}

/*! \brief Test if this feature vector matches parameters from presence.
    \param node Client node identifier.
    \param ver Version identifier or hash string.
    \param hash Hashing algorithm used for hash string.
    \param ext Extensions present on client.
    \return true if parameters are not same as passed, false if they are same.

    Used to check if we need update this feature list. 
    */
bool FeatureList::hasFeaturesChanged(const QString &node, const QString &ver, const QString &hash, const QString &ext)
{
    bool same = (  m_node.compare(node)==0 
        && m_ver.compare(ver)==0
        && m_hash.compare(hash)==0
        && m_exthistory.compare(ext)==0
    );
    bool defined = ( !ver.isEmpty() );
    return (defined && !same);
}

/*! \brief Check if parameter with space delimited extensions changed since last time.
    \param ext Space delimited list of extensions
    \return true if parameter is different from last used extensions.
*/
bool FeatureList::hasExtensionsChanged(const QString &ext)
{
    return (m_exthistory == ext);
}

/*! \brief Set hash algorithm and resulting hash. */
void FeatureList::setHash(const QString &algo, const QString &hash)
{
    m_ver = hash;
    m_hash = algo;
}


/*! \brief Create hexadecimal coded string with hash of hashable string contained. 
    \return Hex encoded result, or QString::null if error occured. */
QString FeatureList::sha1Hash() const
{
    return computeHash("sha1");
}

/*! \brief Create hexadecimal coded string with hash of hashable string contained. 
    \param algo Algorithm to be used for hash. It might be sha1, md5, or anything that QCA supports.
    \return Hex encoded result, or QString::null if error occured. */
QString FeatureList::computeHash(const QString &algo) const
{
    if (!m_known)
        return QString::Null();
    QByteArray algo_c = algo.toAscii();
    if (!QCA::isSupported(algo_c.constData())) {
        return QString::Null();
    }
    QCA::Hash hash(algo);
    // XMPP is defined to use UTF-8, so hardcode it.
    // i expect only ascii characters should be here, but you never know.
    QByteArray tohash = m_hashable.toUtf8();
    return hash.hashToString(tohash);

}


/*! \brief Returns all supported features in one list. */
QStringList FeatureList::allFeatures() const
{
    QStringList features;
    for (const_iterator it=begin(); it!= end(); ++it) {
        if (it.value())
            features.append(it.key());
    }
    return features;
}

bool FeatureList::isLegacy() const
{
    return m_hash.isEmpty();
}

/** \brief Create XML representation of one FeatureList. */
QDomElement FeatureList::toXml(QDomDocument &doc) const
{
    QDomElement root = doc.createElement("features");
    root.setAttribute("node", m_node);
    root.setAttribute("ver", m_ver);
    if (!m_exthistory.isEmpty())
        root.setAttribute("ext", m_exthistory);
    if (!m_hash.isEmpty())
        root.setAttribute("hash", m_hash);

    for (const_iterator it=begin(); it!=end(); ++it) {
        QDomText text = doc.createTextNode(it.key());
        QDomElement element = doc.createElement("feature");
        element.appendChild(text);
        root.appendChild(element);
    }

    if (m_query.hasChildNodes()) {
        QDomElement qelem = doc.createElement("query");
        QDomNode n = doc.importNode( m_query.firstChild(), true );
        qelem.appendChild(n);
        root.appendChild(qelem);
    }
    return root;
}

/** \brief Initialize this structure from passed XML element.
    Intended to read features from cache. */
bool FeatureList::fromXml(QDomElement &element)
{
    if (element.tagName() != "features")
        return false;
    m_node = element.attribute("node");
    m_ver = element.attribute("ver");
    m_hash = element.attribute("hash");
    parseExtensions(element.attribute("ext"));

    for(QDomElement e=element.firstChildElement(); !e.isNull(); e=e.nextSiblingElement()) {
        if (e.tagName() == "feature") {
            QString text = e.text();
            insert(text, true);
        } else if (e.tagName() == "query" ) {
            m_query.clear();
            QDomNode query = m_query.importNode(e, true);
            m_query.appendChild(query);
        } else {
            qWarning() << "Unsupported element " << e.tagName() << " when reading features";
        }
    }
    return true;
}

/*
 * FeatureListManager
 */
FeatureListManager::FeatureListManager(Makneto *makneto)
    : m_makneto(makneto), m_modified(false)
{
}


QDomDocument FeatureListManager::createXMLDatabase()
{
    QDomDocument doc;

    QDomElement root = doc.createElementNS("urn:makneto:capabilites:cache", "capabilities");
    doc.appendChild(root);

    for (HashedFeatures::const_iterator db=m_database.begin();
            db != m_database.end(); db++ ) {
        QDomElement algorithm = doc.createElement("algorithm");
        if (db.key().isEmpty()) {
            algorithm.setAttribute("legacy", "1");
        } else {
            algorithm.setAttribute("type", db.key());
        }

        for (ListOfFeatures::const_iterator it=db->begin();
                it!=db->end(); it++) {
            FeatureList *flp = *it;
            QDomElement client = flp->toXml(doc);
            algorithm.appendChild( client );
        }

        root.appendChild( algorithm );
    }
    return doc;
}

void FeatureList::setCapsParams(const QString &node, const QString &ver, const QString &hash, const QString &ext)
{
    m_node = node;
    m_ver = ver;
    m_hash = hash;
    parseExtensions(ext); 
}



/** \brief Method to read cached features for clients
    \param path Path to XML file with cache
    \return true if reading was successful, false in other case.
*/
bool FeatureListManager::readFromFile(const QString &path)
{

    QFile input(path);
    if (input.open(QIODevice::ReadOnly)) {
        QByteArray contents = input.readAll();
        input.close();
        QDomDocument doc;
        if (!doc.setContent(&input))
            return false;

        QDomElement root = doc.firstChildElement("capabilities");

        if (!root.isNull()) {
            int count = 0;
            int algorithms = 0;
            for (QDomElement algo=root.firstChildElement("algorithm");
                !algo.isNull(); algo=algo.nextSiblingElement()) {
                QString type = algo.attribute("type");

                for (QDomElement features=algo.firstChildElement("features"); 
                    !features.isNull(); features=features.nextSiblingElement("features")) {
                    FeatureList *fl = new FeatureList();
                    if (!fl->fromXml(features)) {
                        qWarning("Reading of feature list failed!");
                        delete fl;
                        return false;
                    }
                    addFeatures(fl);
                    ++count;
                }
                ++algorithms;
            }
            qDebug() << "Finished reading capabilities cache, read " << count << " entries";
            m_modified = false;
            return true;
        } else {
            qWarning("Invalid capabilities cache format");
            return false;
        }

    } else {
        qWarning() << "Failed to open read-only file " << path;
        return false;
    }
    return true;
}

// FIXME: zadna obsluha chyb, nutno spravit
bool FeatureListManager::writeToFile(const QString &path)
{
    FILE *output = fopen(path.toUtf8().constData(), "w");
    if (!output)
        return false;
    QTextStream stream(output);
    stream << createXMLDatabase();
    fclose(output);
    m_modified = false;
    return true;
}

/*! brief Get features class for specifed hash and algorithm. */
FeatureList * FeatureListManager::getFeaturesByHash( const QString &ver, const QString &hash)
{
    QHash<QString, ListOfFeatures>::iterator db = m_database.find(hash);
    if (db != m_database.end()) {
        ListOfFeatures::iterator featuresIt = db->find(ver);
        if (featuresIt != db->end())
            return (featuresIt.value());
        else return NULL;
    } else {
        qDebug() << "Algorithm " << hash << " has no database to search.";
    }
    return NULL;
}

/*! \brief Get features with specified parameters.
    \return Pointer to FeatureList class if they are known, NULL otherwise.
    */
FeatureList * FeatureListManager::getFeatures( const QString &node, const QString &ver, const QString &hash, const QString &ext)
{

    QString index = singleCaps(node, ver, hash, ext);
    
    QHash<QString, ListOfFeatures>::iterator db = m_database.find(hash);
    if (db != m_database.end()) {
        ListOfFeatures::iterator featuresIt = db->find(index);
        if (featuresIt != db->end())
            return (featuresIt.value());
        else return NULL;
    } else {
        qDebug() << "Algorithm " << hash << " has no database to search.";
    }
    return NULL;
}


/*! \brief Check if specified client is know for features. 
    \param ver Version of client in legacy mode, or hash string for up-to-date mode.
    \param node Node of client, hopefully some description.
    \param hash Algorithm used to create ver parameter, or empty if legacy is used.
    \param ext Extensions list used in legacy mode. hash must be empty if this is not.
*/
bool FeatureListManager::isKnown(const QString &node, const QString &ver, const QString &hash, const QString & ext)
{
    Q_UNUSED(node);
    QHash<QString, ListOfFeatures>::iterator db = m_database.find(hash);
    if (db != m_database.end()) {
        QString index = singleCaps(node, ver, hash, ext);
        ListOfFeatures::iterator featuresIt = db->find(index);
        return (featuresIt != db->end());
    } else {
        qDebug() << "Algorithm " << hash << " has no database to search.";
        return false;
    }
    return false;
}

/*! \brief Create new instance and insert it into database. */
void FeatureListManager::addFeatures(const FeatureList &fl)
{
    FeatureList *flp = new FeatureList(fl);
    QString hash;
    hash = flp->sha1Hash();
    m_database["sha-1"].insert(hash, flp);
    qDebug() << "Adding feature list " << fl.node() << fl.ver() << " to manager.";
    m_modified = true;
}

/*! \brief Insert this instance to database.
    Try to not delete it after inserting, it is considered made for feature manager
    and should not be deleted in other places. 
    */
void FeatureListManager::addFeatures(FeatureList *fl)
{
    QString algo = fl->hash();
    QString index = singleCaps(fl->node(), fl->ver(), fl->hash(), fl->ext());
    m_database[algo].insert(index, fl);
    qDebug() << "Adding feature list " << fl->node() << fl->ver() << " with index " << index << " to manager.";
    m_modified = true;
}

/*! \brief Handle one update request finished, add new features to manager. */
void FeatureListManager::requestFinished(FeaturesUpdateRequest *ur)
{
    if (ur->features()->known()) {
        //FeatureList adding(ur->features());
        addFeatures(ur->features());
    }
}

void FeatureListManager::removeRequest(FeaturesUpdateRequest *ur)
{
    for (RequestsHash::iterator it=m_requests.begin(); it!=m_requests.end(); it++) {
        if (it.value() == ur) {
            m_requests.erase(it);
        }
    }

    delete ur;
}

/*! \brief Request new set of queries to specified client.
    \param client Active connection to XMPP server.
    \param jid Target JID of who we want to know features.
    \param node Node, client specifier of some sort.
    \param ver Version string, might be hash or text version in legacy mode.
    \param hash Algorithm used to create hash. In legacy mode it will be empty.
    \param ext List of extensions used by client. Only allowed for legacy mode, so one of hash or ext must be empty. 
    After all replies arrives, it will fire requestFinished()
*/
void FeatureListManager::requestFeatureUpdate(XMPP::Client *client, const XMPP::Jid &jid, const QString &node, const QString &ver, const QString &hash, const QString & ext)
{
    FeaturesUpdateRequest *request = NULL;
    FeatureList *features = new FeatureList();

    request = findRequest(node, ver, hash, ext);
    if (request == NULL) {
        request = new FeaturesUpdateRequest( client, features, jid.full());
        request->setCapsParams(node, ver, hash, ext);
        connect(request, SIGNAL(finished(FeaturesUpdateRequest *)), this, SLOT(requestFinished(FeaturesUpdateRequest *)) );
        request->go();
    } else {
        request->addJid(jid);
    }
    qDebug() << "Requesting features from " << jid.full() << " node " << node << " ver " << ver;
}


void FeatureListManager::requestFeatureUpdate(const XMPP::Jid &jid, const QString &node, const QString &ver, const QString &hash, const QString & ext)
{
    XMPP::Client *client = m_makneto->getConnection()->client();
    requestFeatureUpdate(client, jid, node, ver, hash, ext);
}


FeaturesUpdateRequest *FeatureListManager::findRequest( const QString &node, const QString &ver, const QString &hash, const QString & ext)
{
    QString single = singleCaps(node, ver, hash, ext);
    RequestsHash::iterator it=m_requests.find(single);
    if (it != m_requests.end()) {
        return it.value();
    } else {
        return NULL;
    }
}

QString FeatureListManager::singleCaps( const QString &node, const QString &ver, const QString &hash, const QString & ext)
{
    return node + "#" + ver + "#" + hash + "#" + ext;
}

void FeatureListManager::writeDatabase()
{
    QString path = getDatabasePath();
    QFileInfo info(path);
    if (!info.exists()) {
        QDir dir = info.dir();
        if (!dir.exists()) {
            qDebug() << "Configuration directory does not exist, trying to create: " << dir.path();
            if (!dir.mkpath( dir.path())) {
                qWarning("Creating directory path for cache failed.");
            }
        }
    }
    if (!writeToFile(path)) {
        qWarning() << "Feature cache write failed: "  << path;
    }
}

/** \brief Get path to cache file for current user. */
QString FeatureListManager::getDatabasePath()
{
    QDir home = QDir::home();
    QString path = ".kde/share/apps/makneto";
    QString file("features-cache.xml");
    QString complete = home.path()+QDir::separator()+path+QDir::separator()+file;
    return complete;
}

/** \brief Load cache from local configuration file. */
bool FeatureListManager::readDatabase()
{
    return readFromFile(getDatabasePath());
}

