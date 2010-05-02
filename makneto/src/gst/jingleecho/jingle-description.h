#ifndef JINGLE_DESCRIPTION_H
#define JINGLE_DESCRIPTION_H

#include <string>
#include <list>

#include "payloadparameter.h"

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
        MEDIA_VIDEO = (1 << 1),
        MEDIA_AUDIOVIDEO = (MEDIA_AUDIO|MEDIA_VIDEO)
} MediaType;

/** Class describing one RTP payload for session negotiation.
    @see http://xmpp.org/extensions/xep-0167.html
	It describes one format the client is able to handle for RTP session
    */
class JingleRtpPayload
{
    public:
    typedef std::list<PayloadParameter> ParameterList;

        JingleRtpPayload(unsigned char id, const std::string &name, unsigned int clockrate=8000, int channels=1);
        virtual ~JingleRtpPayload();
		
    unsigned char   id; ///<! required
    std::string     name; ///!< recommended for static, required for dynamic
    unsigned int    clockrate; ///!< recommended
    int             channels; ///!< recommended
    unsigned int    maxptime; ///!< optional
    unsigned int    ptime; ///!< optional

    ParameterList   parameters;

    void add(const PayloadParameter &p); 
	
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

typedef std::list<JingleRtpPayload> PayloadList;

/** @brief Class for description of one content transfered over RTP protocol.
*/
class JingleRtpContentDescription : public JingleContentDescription
{
    public:


        JingleRtpContentDescription();
        virtual ~JingleRtpContentDescription();
        	
	void addPayload(const JingleRtpPayload &payload);
        void clearPayload();
        int countPayload() const;

#ifdef GLOOX	
	virtual void parse(const gloox::Tag *tag);
        virtual gloox::Tag *tag() const;
#else
        virtual void parse(const QDomElement &tag);
        virtual QDomElement tag(QDomDocument &doc) const;
#endif

        std::string media() const;
        std::string xmlns() const;
        MediaType   type() const;
        void setType(MediaType type);

    PayloadList payloads;
	std::string	m_xmlns;
	std::string m_media;
        MediaType   m_type;
};



#endif

