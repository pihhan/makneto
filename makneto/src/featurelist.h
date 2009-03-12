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

#include <QHash>
#include <QDomElement>
#include <QtAlgorithms>
#include <xmpp_stanza.h>

/*! \brief Helper class implementing requested order of sorting for multiple identities */
class IdentityHelper
{
    public:
        /*! \brief Create structure using element with tagName == "identity" */
        IdentityHelper(QDomElement &element)
        {
            category = element.attribute("category");
            type = element.attribute("type");
            lang = element.attribute("xml:lang");
            name = element.attribute("name");
        }

        IdentityHelper(const QString &category, const QString &type, const QString &name, const QString &lang) 
            : category(category), name(name),  lang(lang), type(type)
        {
        }

        /*! \brief Returns string with format of one identity for caps hashing. */
        QString capsHashable() const
        {
            return category + "/" + type + "/" + lang + "/" + name + "<";
        }

        bool operator>(const IdentityHelper &other) 
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
        bool operator<(const IdentityHelper &other) const
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

    private:
        QString category;
        QString name;
        QString lang;
        QString type;
};

/*! \brief Helper class for sorting single fields of form.
 * This class does containg one field with one class. It allows sorting
 * based on var attribute. */
class XFormFieldHelper
{
    public:

        typedef QList<XFormFieldHelper> List;
        /*! \brief Create structure for one variable in form.
         * \param element element with <field> tag. */
        XFormFieldHelper(const QDomElement &element)
        {
            m_var = element.attribute("var");
            m_element = QDomElement(element);

        }

        bool operator<(const XFormFieldHelper &other)
        {
            return (m_var < other.m_var);
        }

        /*! \brief Returns all values of added field. 
         * It does not sort, it returns strings in original order. */
        QStringList values()
        {
            QStringList values;
            for (QDomElement child = m_element.firstChildElement("value");
                !child.isNull(); child = child.nextSiblingElement()) {
                if (!child.text().isEmpty())
                    values.append(child.text());

            }
            return values;
        }

        /*! \brief Returns only first value for this field.
         *
         * \return first value for this field.
         * It is enough for most cases, as most forms have only one value for one variable. */
        QString firstValue()
        {
            QDomElement value = m_element.firstChildElement("value");
            return value.text();
        }

        /*! \brief Returns string for caps hashing for this field's values.
         * No fieldname is included. */
        QString capsHashableValues()
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
        QString capsHashable()
        {
            QString result = m_var + "<";
            result.append( capsHashableValues() );
            return result;
        }

        /*! \brief Returns keyword for this field. */
        QString key()
        {
            return m_var;
        }

        QDomElement element()
        { return m_element; }

    private:
        QDomElement m_element;
        QString m_var;
};



/*! \brief Helper class containing one form.
 * It allows sorting several forms by their type, as needed for XEP-115. */
class XFormHelper
{
    public:
        typedef QList<XFormHelper>  List;
        
        XFormHelper() {}

        /*! \brief Create XFormHelper using "x" element with xmlns="form:x:data" */
        XFormHelper(const QDomElement &element)
        {
            m_form_type = lookupFormType(element);
            m_element = QDomElement(element);
        }

        QString lookupFormType(const QDomElement &parent) const
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

        QString capsHashableFormType()
        {
            return m_form_type + "<";
        }

        bool operator<(const XFormHelper &other)
        {
            return m_form_type < other.m_form_type;
        }

        QDomElement element()
        { return m_element; }

    private:
        QString m_form_type;
        QDomElement m_element;
};



/*! \brief Identity sorter with helper to output. */
class IdentitySorter 
{
    public:
        typedef QList<IdentityHelper>   IdentityList;

        IdentitySorter()
        {}

        /*! \brief Creates structure with passed argument as first element. */
        IdentitySorter(const IdentityHelper &identity)
        {
            m_list.append(identity);
        }

        void add(IdentityHelper &identity)
        {
            m_list.append(identity);
        }

        void sort()
        {
            qSort(m_list.begin(), m_list.end());
        }

        /*! \brief Returns one string with all identities formated to in caps format in proper order. 
         * It does sort for you also. */
        QString capsHashable()
        {
            QString hash;
            sort();
            for (IdentityList::const_iterator it = m_list.begin(); it != m_list.end(); it++) {
                hash += it->capsHashable();
            }
            return hash;
        }

    private:
        IdentityList    m_list;
};



class FeatureList : public QHash<QString, bool>
{
    public:
        FeatureList() : m_known(true)
        {
        }

        /*! \brief Do we know features of this owner? */
        bool known()
        {
            return m_known;
        }

        /*! \brief Is feature supported?
         * \param xmlns XML namespace of feature, as registered at http://xmpp.org/registrar/namespaces.html */
        bool supported(const QString & xmlns)
        {
            return contains(xmlns);
        }

        void addFeature(const QString & feature)
        {
            insert(feature, true);
            m_known = true;
        }

        void addFeatures(const QStringList &features)
        {
            QStringList::const_iterator it;
            for (it= features.begin(); it!=features.end(); it++) {
                addFeature(*it);
            }
        }

        QString computeHashSha1(XMPP::Stanza *stanza) const
        {
            QString appended;
            QStringList identities;
            QStringList features;
            QDomDocument doc;
            QDomNode query;
            QDomElement identity;

            query = stanza->doc().firstChildElement("query");
            for (QDomElement child = query.firstChildElement();
                    !child.isNull(); child = child.nextSiblingElement()) {
                if (child.tagName() == "identity") {
                    QString si;
                    si = child.attribute("category") + "/";
                    si += child.attribute("type") + "/";
                    si += child.attribute("xml:lang") + "/";
                    si += child.attribute("name") + "<";
                    identities.append(si);
                } else if (child.tagName() == "feature") {
                    QString sf = child.text() + "<";
                    features.append(sf);
                } // do not try to understand other childen, we dont need them now
            }

            identities.sort();
            features.sort();

        }

    private:

    bool m_known; ///<! Are features of this client known?
    
};

/*! \brief Simple database for capabilities of clients and mapping to its hashes. */
class FeatureListManager
{
    public:
        typedef QHash<QString, FeatureList *> ListOfFeatures;

        FeatureListManager()
        {}

        FeatureList * getFeaturesByHash( const QString &hash);


    private:
        ListOfFeatures m_database;
        
};

#endif 

