/*
 * Pomocne tridy pro razeni caps retezcu ve spravnem poradi.
 * Autor: Petr Mensik <pihhan@cipis.net>
 */
#ifndef FEATUREHELPERS_H
#define FEATUREHELPERS_H

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

        void loadFields(const QDomElement &parent);

        void sortFields()
        {
            qSort(m_fields.begin(), m_fields.end());
        }

        QString capsHashableFormType() const
        {
            return m_form_type + "<";
        }

        QString capsHashable();

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



#endif
