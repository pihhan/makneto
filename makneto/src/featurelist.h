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
#include <QStringList>
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

        bool operator>(const IdentityHelper &other) const;

        /*! \brief Operator needed for sorting order according to XEP-115 */
        bool operator<(const IdentityHelper &other) const;

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

        bool operator<(const XFormFieldHelper &other) const
        {
            return (m_var < other.m_var);
        }

        /*! \brief Returns all values of added field. 
         * It does not sort, it returns strings in original order. */
        QStringList values();

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
        QString capsHashableValues();

        /*! \brief Returns string hashing for whole field, with values and keyname included. */
        QString capsHashable();

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

        QString lookupFormType(const QDomElement &parent) const;

        void loadFields(const QDomElement &parent) 
        {
            for (QDomElement e=parent.firstChildElement("field");
                        !e.isNull(); e=e.nextSiblingElement("field")) {
                m_fields.append(XFormFieldHelper(e));
            }

        }

        void sortFields()
        {
            qSort(m_fields.begin(), m_fields.end());
        }

        QString capsHashableFormType() const
        {
            return m_form_type + "<";
        }

        /*! \brief Returns part of string to hash for whole form. */
        QString capsHashable()
        {
            QString hash(capsHashableFormType());
            sortFields();
            for (XFormFieldHelper::List::iterator it= m_fields.begin();
                    it != m_fields.end(); it++) {
                hash.append( it->capsHashable() );
            }
            return hash;
        }

        bool operator<(const XFormHelper &other) const
        {
            return m_form_type < other.m_form_type;
        }

        QDomElement element()
        { return m_element; }

    private:
        QString m_form_type;
        QDomElement m_element;
        XFormFieldHelper::List  m_fields;
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

        void add(const IdentityHelper &identity)
        {
            m_list.append(identity);
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


/*! \brief Class for managing capabilites of one client. */
class FeatureList : public QHash<QString, bool>
{
    public:
        FeatureList() : m_known(true)
        {
        }

        /*! \brief Create Feature list from Disco#identity reply. */
        FeatureList(QDomElement &query)
            : m_known(true)
        {
            m_query = QDomElement(query);
            m_hash = computeHashString(query);
            for (QDomElement e=query.firstChildElement("feature"); !e.isNull();
                    e.nextSiblingElement("feature")) {
                addFeature(e.text());
            }
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
            m_known = true;
        }

        void setHash(const QString &hash)
        {
            m_hash = hash;
        }

        QString capsHash() const
        {
            return m_hash;
        }

        QDomElement query()
        {
            return m_query;
        }

        static QString capsHashableStringList(const QStringList &list);

        /*! \brief Create part of XEP-155 verify string from forms included in list.
         * list is sorted in this method, it is modified then. */
        static QString capsHashableForms(XFormHelper::List &list);

        /*! \brief Create XEP-115 hash string from iq:query element.
         * \param query Child of iq stanza query, with xmlns of disco#info */
        static QString computeHashString(QDomElement &query);

    private:

    bool m_known; ///<! Are features of this client known?
    QString m_hash;
    QDomElement m_query;
    
};

/*! \brief Simple database for capabilities of clients and mapping to its hashes. */
class FeatureListManager
{
    public:
        typedef QHash<QString, FeatureList *> ListOfFeatures;

        FeatureListManager()
        {}

        FeatureList * getFeaturesByHash( const QString &hash);

        /* FIXME: tohle nebude fungovat spravne. nemam jeste pocitani skutecneho hashe. */
        void addFeatures(const FeatureList &fl)
        {
            FeatureList *flp = new FeatureList(fl);
            QString hash;
            hash = flp->capsHash();
            m_database.insert(hash, flp);
        }

        /*! \brief Reads features cache from xml file. */
        void readFromFile(const QString &path);

        /*! \brief Create XML database structure for saving cache. */
        QDomDocument createXMLDatabase();

        void writeToFile(const QString &path);


    private:
        ListOfFeatures m_database;
        
};

#endif 

