
#include <xmpp_xmlcommon.h>
#include <xmpp_jid.h>
#include <xmpp_client.h>
#include <QDebug>

#include "discorequest.h"
#include "featurelist.h"


DiscoRequest::DiscoRequest(Task *parent, FeatureListManager *manager)
    : XMPP::Task(parent), m_flm(manager), m_features()
{
    connect(this, SIGNAL(finished()), this, SLOT(updateFeatureManager()) );
}

DiscoRequest::DiscoRequest(XMPP::Task *parent, FeatureList *features)
    : XMPP::Task(parent), m_flm(NULL), m_features(features)
{
}


/*! \brief Set target JID and node of sending query. */
void DiscoRequest::get(const QString &jid, const QString &node)
{
    m_jid = jid;
    m_node = node;
}

/*! \brief Parse received reply to our request.
    \param element Stanza element we received.

    After parsing information, we emit finished() signal.
*/
bool DiscoRequest::take(const QDomElement &element)
{
    if (!iqVerify(element, XMPP::Jid(m_jid), id()) ) {
        return false;
    }
    qDebug() << "DiscoRequest::take entered, verified.";
    

    QDomElement query = element.firstChildElement("query");
    if (!query.isElement()) {
        qDebug() << "query element missing";
        return true;
    }

    if (element.attribute("type") == "result") {
        if (m_features) 
            m_features->update(query);
        else 
            m_features = new FeatureList(query);
        setSuccess();
        qDebug() << "Disco success";
    } else {
        setError(element);
        qWarning() << "Disco error";
    }

    return true;
}

/*! \brief After we finish task and have parsed reply, update information in
    feature manager, if we have it set */
void DiscoRequest::updateFeatureManager()
{
    if (success() && m_flm != NULL && m_features != NULL) {
        m_flm->addFeatures(*m_features);
    }
    
    qDebug() << "DiscoRequest::updateFeatureManager trigger" << (m_features ? m_features->capsHashable() : "NULL");
}

void DiscoRequest::setFeatureManager(FeatureListManager *manager)
{
    m_flm = manager;
}

/*! \brief What will we do after doing go().
    It will send request we prepared using get().
*/
void DiscoRequest::onGo()
{
    QDomElement iq = createIQ(doc(), "get", m_jid, id());

    QDomElement query = doc()->createElement("query");
    query.setAttribute("xmlns", XMLNS_DISCO_INFO);
    if (!m_node.isEmpty()) {
        query.setAttribute("node", m_node);
    }
    iq.appendChild(query);

    send(iq);
}

void DiscoRequest::onDisconnect()
{
    qWarning() << "DiscoRequest::onDisconnect";
}



/*
 * 
 *  FeatureUpdateRequest
 *
 */

FeaturesUpdateRequest::FeaturesUpdateRequest(XMPP::Client *client, FeatureList *features, const QString &jid)
    : m_client(client),m_features(features),m_requestspending(0),m_errorcode(0)
{
    m_jid = XMPP::Jid(jid);
    m_jids.append(XMPP::Jid(jid));
}

void FeaturesUpdateRequest::setCapsParams(const QString &node, const QString &ver, const QString &hash, const QString &ext)
{
    m_features->setCapsParams(node, ver, hash, ext);
}

/*! \brief Send requests. */
void FeaturesUpdateRequest::go()
{
    // find features for every extension
    QStringList extensions = m_features->extensions();
    for (QStringList::iterator it=extensions.begin(); it!=extensions.end(); ++it) {
        DiscoNodeRequest(*it);
    }

    DiscoNodeRequest(m_features->ver());
}
    
/*! \brief Wait until all request arrive, then fire finished signal. */ 
void FeaturesUpdateRequest::oneRequestFinished()
{
    if (--m_requestspending <= 0) {
        emit finished(this);
    }
}


/*! \brief Create disco request using node in features and subnode used as extension.
    It will create node in disco#info request as node#subnode, where node is client identifier,
    subnode might be extension for legacy caps or version hash for newer.
    */
DiscoRequest * FeaturesUpdateRequest::DiscoNodeRequest(const QString &subnode)
{
        DiscoRequest * request = new DiscoRequest(m_client->rootTask(), m_features);
        QString node = m_features->node() + "#" + subnode;
        request->get(m_jid.full(), node);
        ++m_requestspending;
        connect(request, SIGNAL(finished()), this, SLOT(oneRequestFinished()) );
        request->go(true);
        return request;
}

/*! \brief Add one JID to list of interested contacts for result of this query. */
void FeaturesUpdateRequest::addJid(const XMPP::Jid &jid)
{
    m_jids.append(jid);
}

