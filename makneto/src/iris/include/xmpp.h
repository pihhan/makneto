/*
 * xmpp.h - XMPP "core" library API
 * Copyright (C) 2003  Justin Karneges
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef XMPP_H
#define XMPP_H

#include <QPair>
#include <qobject.h>
#include <qstring.h>
#include <qhostaddress.h>
#include <qstring.h>
#include <q3cstring.h>
#include <qxml.h>
#include <qdom.h>
#include <qglobal.h>

#include "xmpp_jid.h"
#include "xmpp_stanza.h"
#include "xmpp_stream.h"
#include "xmpp_clientstream.h"

namespace QCA
{
	class TLS;
};

#ifndef CS_XMPP
class ByteStream;
#endif

#include <QtCrypto> // For QCA::SASL::Params

namespace XMPP
{
	// CS_IMPORT_BEGIN cutestuff/bytestream.h
#ifdef CS_XMPP
	class ByteStream;
#endif
	// CS_IMPORT_END

	class Debug
	{
	public:
		virtual ~Debug();

		virtual void msg(const QString &)=0;
		virtual void outgoingTag(const QString &)=0;
		virtual void incomingTag(const QString &)=0;
		virtual void outgoingXml(const QDomElement &)=0;
		virtual void incomingXml(const QDomElement &)=0;
	};

	void setDebug(Debug *);

        /*! \brief Class for connection management and setup. 
         * This class has only basic features, see AdvancedConnector if you 
         * need proxy or HTTP polling support. */
	class Connector : public QObject
	{
		Q_OBJECT
	public:
		Connector(QObject *parent=0);
		virtual ~Connector();
                
                /*! \brief Start connection to server.
                 * \param server DNS name of server.
                 * */
		virtual void connectToServer(const QString &server)=0;
		virtual ByteStream *stream() const=0;
		virtual void done()=0;

                /*! \brief Get configuration of SSL.
                 * \return True if we will connect with secure stream. */
		bool useSSL() const;
                /*! \brief Do we know address?.
                 * \return True if IP address is known already. */
		bool havePeerAddress() const;
		QHostAddress peerAddress() const;
		quint16 peerPort() const;

	signals:
		void connected();
		void error();

	protected:
                /*! \brief Configure usage of secure stream. 
                 * Force SSL encryption right after connection, legacy SSL.
                 * \param b True if we should try SSL on connection. */
		void setUseSSL(bool b);
                /*! \brief Clear configured server IP address. */
		void setPeerAddressNone();
                /*! \brief Configure server IP address. 
                 * \param addr IP addres of server. 
                 * \param port Target port of server. */
		void setPeerAddress(const QHostAddress &addr, quint16 port);

	private:
		bool ssl;
		bool haveaddr;
		QHostAddress addr;
		quint16 port;
	};
        
        /*! \brief Advanced connector with special connections also.
         *
         * It does support connection using HTTP proxy and SOCKS proxy. */
	class AdvancedConnector : public Connector
	{
		Q_OBJECT
	public:
		enum Error { ErrConnectionRefused, ErrHostNotFound, ErrProxyConnect, ErrProxyNeg, ErrProxyAuth, ErrStream };
		AdvancedConnector(QObject *parent=0);
		virtual ~AdvancedConnector();

		class Proxy
		{
		public:
			enum { None, HttpConnect, HttpPoll, Socks };
			Proxy();
			~Proxy();

			int type() const;
			QString host() const;
			quint16 port() const;
			QString url() const;
			QString user() const;
			QString pass() const;
			int pollInterval() const;

                        /*! \brief Set connection type as HTTP using CONNECT request. 
                         *
                         * Connect is faster and more reliable than polling, 
                         * but have to be supported and enabled by Proxy servers,
                         * often is disabled by default. After HTTP request does work like normal TCP connection.
                         * \param host DNS name of HTTP server. 
                         * \param port Port of HTTP server. */
			void setHttpConnect(const QString &host, quint16 port);
                        /*! \brief Set connection type to HTTP polling.
                         *
                         * Polling is slower, less efficient and does use more
                         * traffic than connect method. But it does look like
                         * normal HTTP traffic and should pass any proxy.
                         * \param host DNS name of HTTP server.
                         * \param port Port of HTTP server.
                         * \param url Url to polling page. FIXME: is relative to server root, or full URL with http:// ? */
			void setHttpPoll(const QString &host, quint16 port, const QString &url);
			void setSocks(const QString &host, quint16 port);
                        /*! \brief Set credentials for authentication. 
                         * \param user Username. 
                         * \param pass Password. */
			void setUserPass(const QString &user, const QString &pass);
                        /*! \brief Set polling interval.
                         * It does affect time of reaction. Lesser the time is,
                         *  faster and more instant messages are, but also uses
                         *  more network resources and data. */
			void setPollInterval(int secs);

		private:
			int t;
			QString v_host, v_url;
			quint16 v_port;
			QString v_user, v_pass;
			int v_poll;
		};

                /*! \brief Set proxy server.
                 * Does work only if connection is idle. */
		void setProxy(const Proxy &proxy);
                /*! \brief Set manual IP hostname and port.
                 * Does work only if connection is idle. */
		void setOptHostPort(const QString &host, quint16 port);
                /*! \brief Configure SSL probing. 
                 * Does work only if connection is idle. Check this, if legacy
                 * SSL connection should be tried first. If that would not work,
                 * try plain TCP connection instead. */
		void setOptProbe(bool);
                /*! \brief Set SSL usage.
                 * Does work only if connection is idle. */
		void setOptSSL(bool);

		void changePollInterval(int secs);

                /*! \brief Start connection to server.
                 * \param server DNS name of server. */
		void connectToServer(const QString &server);
		ByteStream *stream() const;
		void done();

		int errorCode() const;

	signals:
		void srvLookup(const QString &server);
		void srvResult(bool success);
		void httpSyncStarted();
		void httpSyncFinished();

	private slots:
                /*! \brief DNS resolution is complete, we have target address. */
		void dns_done();
                /*! \brief Triggers when SRV reply arrives.
                 * Set hostname and port.
                 * */
		void srv_done();
		void bs_connected();
                /*! \brief Handle errors, or try next SRV hostname if we have any */
		void bs_error(int);
		void http_syncStarted();
		void http_syncFinished();

	private:
		class Private;
		Private *d;

		void cleanup();
		void do_resolve();
                /*! \brief Initiate actual connection. 
                 * Connect to XMPP or Proxy server. On success, signal to 
                 * bs_connected is emited, on error to bs_error.*/
		void do_connect();
                /*! \brief Try next server from SRV reply list. */
		void tryNextSrv();
	};


        /*! \brief Virtual abstraction of TLS handler. */
	class TLSHandler : public QObject
	{
		Q_OBJECT
	public:
		TLSHandler(QObject *parent=0);
		virtual ~TLSHandler();

		virtual void reset()=0;
		virtual void startClient(const QString &host)=0;
		virtual void write(const QByteArray &a)=0;
		virtual void writeIncoming(const QByteArray &a)=0;

	signals:
		void success();
		void fail();
		void closed();
		void readyRead(const QByteArray &a);
		void readyReadOutgoing(const QByteArray &a, int plainBytes);
	};

        /*! \brief Useable and implemented TLS handler. */
	class QCATLSHandler : public TLSHandler
	{
		Q_OBJECT
	public:
		QCATLSHandler(QCA::TLS *parent);
		~QCATLSHandler();

		QCA::TLS *tls() const;
		int tlsError() const;
                
                /*! \brief Enable certificate hostname matching.
                 * \param enable true to enable checking */
		void setXMPPCertCheck(bool enable);
		bool XMPPCertCheck();
		bool certMatchesHostname();

		void reset();
		void startClient(const QString &host);
		void write(const QByteArray &a);
		void writeIncoming(const QByteArray &a);

	signals:
		void tlsHandshaken();

	public slots:
		void continueAfterHandshake();

	private slots:
		void tls_handshaken();
		void tls_readyRead();
		void tls_readyReadOutgoing();
		void tls_closed();
		void tls_error();

	private:
		class Private;
		Private *d;
	};
};

#endif
