
#include "featurelist.h"


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
QString FeatureList::computeHashString(QDomElement &query)
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
        } else if (child.tagName() == "feature") {
            QString sf = child.text() + "<";
            features.append(sf);
        } else if (child.attribute("xmlns") == "jabber:x:data") {
            // formulář s extended info
            forms.append(XFormHelper(child));
        } // do not try to understand other childen, we dont need them now
    }

    hashable = identsort.capsHashable();
    hashable += capsHashableStringList(features);
    hashable += capsHashableForms(forms);
    return hashable; 
}


QDomDocument FeatureListManager::createXMLDatabase()
{
    QDomDocument doc;

    QDomElement root = doc.createElementNS("urn:makneto:capabilites:cache", "capabilites");

    for (ListOfFeatures::const_iterator it=m_database.begin();
            it!=m_database.end(); it++) {
        FeatureList *flp = *it;
        QDomElement client;
        client.setTagName("client");
        client.setAttribute("ver", flp->capsHash());
        client.appendChild( flp->query() );
        root.appendChild( client );

    }
    doc.appendChild(root);
    return doc;
}


void FeatureListManager::readFromFile(const QString &path)
{
    // TODO: to be written
}


void FeatureListManager::writeToFile(const QString &path)
{
}

