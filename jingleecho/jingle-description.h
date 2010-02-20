#ifndef JINGLE_DESCRIPTION_H
#define JINGLE_DESCRIPTION_H

#include <string>
#include <list>

#ifdef GLOOX
#include <gloox/tag.h>
#else // IRIS
#include <QDomDocument>
#include <QDomElement>
#endif

class JingleContentDescription
{
    public:
    std::string xmlns;
};

typedef enum {
        MEDIA_NONE = 0,
        MEDIA_AUDIO = (1 << 0),
        MEDIA_VIDEO = (1 << 1)
} MediaType;

/** @brief Parameter for class JingleRtpPayload. 
    Allow basic type storage. */
class PayloadParameter {
    public:

    typedef enum {
        TYPE_INT,
        TYPE_UINT,
        TYPE_STRING
    } ParamType;

    PayloadParameter(const std::string &name, int value);
    PayloadParameter(const std::string &name, unsigned int value);
    PayloadParameter(const std::string &name, const std::string &value);

    int intValue();
    unsigned int uintValue();
    std::string stringValue();
    ParamType type();

    private:
    int ivalue;
    std::string m_name;
    unsigned int uivalue;
    std::string svalue;
    ParamType   m_type;
};

/** Class describing one RTP payload for session negotiation.
    @see http://xmpp.org/extensions/xep-0167.html
	It describes one format the client is able to handle for RTP session
    */
class JingleRtpPayload
{
    public:
    typedef std::list<PayloadParameter> ParamterList;

        JingleRtpPayload(unsigned char id, const std::string &name, unsigned int clockrate=8000, int channels=1);
		
    unsigned char   id; ///<! required
    std::string     name; ///!< recommended for static, required for dynamic
    unsigned int    clockrate; ///!< recommended
    int             channels; ///!< recommended
    unsigned int    maxptime; ///!< optional
    unsigned int    ptime; ///!< optional


	
#ifdef GLOOX
    JingleRtpPayload(const gloox::Tag *tag);
    void parse(const gloox::Tag *tag);
    gloox::Tag *tag() const;
#else
    JingleRtpPayload(const QDomElement &tag);
    void parse(const QDomElement &tag);
    QDomElement tag(QDomDocument &doc) const;
#endif

};


/** @brief Class for description of one content transfered over RTP protocol.
*/
class JingleRtpContentDescription : public JingleContentDescription
{
    public:

    typedef std::list<JingleRtpPayload> PayloadList;
	
	void addPayload(const JingleRtpPayload &payload) 
        { payloads.push_back(payload); }

#ifdef GLOOX	
	virtual void parse(const gloox::Tag *tag);
        virtual gloox::Tag *tag() const;
#else
        virtual void parse(const QDomElement &tag);
        virtual QDomElement tag(QDomDocument &doc) const;
#endif

        std::string media() const
        { return m_media; }

        std::string xmlns() const
        { return m_xmlns; }

        MediaType   type() const
        { return m_type; }

    PayloadList payloads;
	std::string	m_xmlns;
	std::string m_media;
        MediaType   m_type;
};



#endif

