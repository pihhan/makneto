/*
 * Copyright (C) 2006  Justin Karneges
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301  USA
 *
 */

#include <stdio.h>
#include <iris/processquit.h>
#include <iris/netinterface.h>
#include <iris/netavailability.h>
#include <iris/netnames.h>
#include <iris/stunmessage.h>
#include <iris/stuntransaction.h>
#include <iris/stunbinding.h>
#include <iris/stunallocate.h>
#include <QtCrypto>

using namespace XMPP;

static QString prompt(const QString &s)
{
	printf("* %s ", qPrintable(s));
	fflush(stdout);
	char line[256];
	fgets(line, 255, stdin);
	QString result = line;
	if(result[result.length()-1] == '\n')
		result.truncate(result.length()-1);
	return result;
}

class NetMonitor : public QObject
{
	Q_OBJECT
public:
	NetInterfaceManager *man;
	QList<NetInterface*> ifaces;
	NetAvailability *netavail;

	~NetMonitor()
	{
		delete netavail;
		qDeleteAll(ifaces);
		delete man;
	}

signals:
	void quit();

public slots:
	void start()
	{
		connect(ProcessQuit::instance(), SIGNAL(quit()), SIGNAL(quit()));

		man = new NetInterfaceManager;
		connect(man, SIGNAL(interfaceAvailable(const QString &)),
			SLOT(here(const QString &)));
		QStringList list = man->interfaces();
		for(int n = 0; n < list.count(); ++n)
			here(list[n]);

		netavail = new NetAvailability;
		connect(netavail, SIGNAL(changed(bool)), SLOT(avail(bool)));
		avail(netavail->isAvailable());
	}

	void here(const QString &id)
	{
		NetInterface *iface = new NetInterface(id, man);
		connect(iface, SIGNAL(unavailable()), SLOT(gone()));
		printf("HERE: %s name=[%s]\n", qPrintable(iface->id()), qPrintable(iface->name()));
		QList<QHostAddress> addrs = iface->addresses();
		for(int n = 0; n < addrs.count(); ++n)
			printf("  address: %s\n", qPrintable(addrs[n].toString()));
		if(!iface->gateway().isNull())
			printf("  gateway: %s\n", qPrintable(iface->gateway().toString()));
		ifaces += iface;
	}

	void gone()
	{
		NetInterface *iface = (NetInterface *)sender();
		printf("GONE: %s\n", qPrintable(iface->id()));
		ifaces.removeAll(iface);
		delete iface;
	}

	void avail(bool available)
	{
		if(available)
			printf("** Network available\n");
		else
			printf("** Network unavailable\n");
	}
};

static QString dataToString(const QByteArray &buf)
{
	QString out;
	for(int n = 0; n < buf.size(); ++n)
	{
		unsigned char c = (unsigned char)buf[n];
		if(c == '\\')
			out += "\\\\";
		else if(c >= 0x20 || c < 0x7f)
			out += c;
		else
			out += QString("\\x%1").arg((uint)c, 2, 16);
	}
	return out;
}

static void print_record(const NameRecord &r)
{
	switch(r.type())
	{
		case NameRecord::A:
			printf("A: [%s] (ttl=%d)\n", qPrintable(r.address().toString()), r.ttl());
			break;
		case NameRecord::Aaaa:
			printf("AAAA: [%s] (ttl=%d)\n", qPrintable(r.address().toString()), r.ttl());
			break;
		case NameRecord::Mx:
			printf("MX: [%s] priority=%d (ttl=%d)\n", r.name().data(), r.priority(), r.ttl());
			break;
		case NameRecord::Srv:
			printf("SRV: [%s] port=%d priority=%d weight=%d (ttl=%d)\n", r.name().data(), r.port(), r.priority(), r.weight(), r.ttl());
			break;
		case NameRecord::Ptr:
			printf("PTR: [%s] (ttl=%d)\n", r.name().data(), r.ttl());
			break;
		case NameRecord::Txt:
		{
			QList<QByteArray> texts = r.texts();
			printf("TXT: count=%d (ttl=%d)\n", texts.count(), r.ttl());
			for(int n = 0; n < texts.count(); ++n)
				printf("  len=%d [%s]\n", texts[n].size(), qPrintable(dataToString(texts[n])));
			break;
		}
		case NameRecord::Hinfo:
			printf("HINFO: [%s] [%s] (ttl=%d)\n", r.cpu().data(), r.os().data(), r.ttl());
			break;
		case NameRecord::Null:
			printf("NULL: %d bytes (ttl=%d)\n", r.rawData().size(), r.ttl());
			break;
		default:
			printf("(Unknown): type=%d (ttl=%d)\n", r.type(), r.ttl());
			break;
	}
}

static int str2rtype(const QString &in)
{
	QString str = in.toLower();
	if(str == "a")
		return NameRecord::A;
	else if(str == "aaaa")
		return NameRecord::Aaaa;
	else if(str == "ptr")
		return NameRecord::Ptr;
	else if(str == "srv")
		return NameRecord::Srv;
	else if(str == "mx")
		return NameRecord::Mx;
	else if(str == "txt")
		return NameRecord::Txt;
	else if(str == "hinfo")
		return NameRecord::Hinfo;
	else if(str == "null")
		return NameRecord::Null;
	else
		return -1;
}

class ResolveName : public QObject
{
	Q_OBJECT
public:
	QString name;
	NameRecord::Type type;
	bool longlived;
	NameResolver dns;
	bool null_dump;

	ResolveName()
	{
		null_dump = false;
	}

public slots:
	void start()
	{
		connect(ProcessQuit::instance(), SIGNAL(quit()), SIGNAL(quit()));

		connect(&dns, SIGNAL(resultsReady(const QList<XMPP::NameRecord> &)),
			SLOT(dns_resultsReady(const QList<XMPP::NameRecord> &)));
		connect(&dns, SIGNAL(error(XMPP::NameResolver::Error)),
			SLOT(dns_error(XMPP::NameResolver::Error)));

		dns.start(name.toLatin1(), type, longlived ? NameResolver::LongLived : NameResolver::Single);
	}

signals:
	void quit();

private slots:
	void dns_resultsReady(const QList<XMPP::NameRecord> &list)
	{
		if(null_dump && list[0].type() == NameRecord::Null)
		{
			QByteArray buf = list[0].rawData();
			fwrite(buf.data(), buf.size(), 1, stdout);
		}
		else
		{
			for(int n = 0; n < list.count(); ++n)
				print_record(list[n]);
		}
		if(!longlived)
		{
			dns.stop();
			emit quit();
		}
	}

	void dns_error(XMPP::NameResolver::Error e)
	{
		QString str;
		if(e == NameResolver::ErrorNoName)
			str = "ErrorNoName";
		else if(e == NameResolver::ErrorTimeout)
			str = "ErrorTimeout";
		else if(e == NameResolver::ErrorNoLocal)
			str = "ErrorNoLocal";
		else if(e == NameResolver::ErrorNoLongLived)
			str = "ErrorNoLongLived";
		else // ErrorGeneric, or anything else
			str = "ErrorGeneric";

		printf("Error: %s\n", qPrintable(str));
		emit quit();
	}
};

class BrowseServices : public QObject
{
	Q_OBJECT
public:
	QString type, domain;
	ServiceBrowser browser;

public slots:
	void start()
	{
		connect(ProcessQuit::instance(), SIGNAL(quit()), SIGNAL(quit()));

		connect(&browser, SIGNAL(instanceAvailable(const XMPP::ServiceInstance &)),
			SLOT(browser_instanceAvailable(const XMPP::ServiceInstance &)));
		connect(&browser, SIGNAL(instanceUnavailable(const XMPP::ServiceInstance &)),
			SLOT(browser_instanceUnavailable(const XMPP::ServiceInstance &)));
		connect(&browser, SIGNAL(error()), SLOT(browser_error()));

		browser.start(type, domain);
	}

signals:
	void quit();

private slots:
	void browser_instanceAvailable(const XMPP::ServiceInstance &i)
	{
		printf("HERE: [%s] (%d attributes)\n", qPrintable(i.instance()), i.attributes().count());
		QMap<QString,QByteArray> attribs = i.attributes();
		QMapIterator<QString,QByteArray> it(attribs);
		while(it.hasNext())
		{
			it.next();
			printf("  [%s] = [%s]\n", qPrintable(it.key()), qPrintable(dataToString(it.value())));
		}
	}

	void browser_instanceUnavailable(const XMPP::ServiceInstance &i)
	{
		printf("GONE: [%s]\n", qPrintable(i.instance()));
	}

	void browser_error()
	{
	}
};

class ResolveService : public QObject
{
	Q_OBJECT
public:
	int mode;
	QString instance;
	QString type;
	QString domain;
	int port;

	ServiceResolver dns;

public slots:
	void start()
	{
		connect(ProcessQuit::instance(), SIGNAL(quit()), SIGNAL(quit()));

		connect(&dns, SIGNAL(resultsReady(const QHostAddress &, int)),
			SLOT(dns_resultsReady(const QHostAddress &, int)));
		connect(&dns, SIGNAL(finished()), SLOT(dns_finished()));
		connect(&dns, SIGNAL(error()), SLOT(dns_error()));

		if(mode == 0)
			dns.startFromInstance(instance.toLatin1() + '.' + type.toLatin1() + ".local.");
		else if(mode == 1)
			dns.startFromDomain(domain, type);
		else // 2
			dns.startFromPlain(domain, port);
	}

signals:
	void quit();

private slots:
	void dns_resultsReady(const QHostAddress &addr, int port)
	{
		printf("[%s] port=%d\n", qPrintable(addr.toString()), port);
		dns.tryNext();
	}

	void dns_finished()
	{
		emit quit();
	}

	void dns_error()
	{
	}
};

class PublishService : public QObject
{
	Q_OBJECT
public:
	QString instance;
	QString type;
	int port;
	QMap<QString,QByteArray> attribs;
	QByteArray extra_null;

	ServiceLocalPublisher pub;

public slots:
	void start()
	{
		//NetInterfaceManager::instance();

		connect(ProcessQuit::instance(), SIGNAL(quit()), SIGNAL(quit()));

		connect(&pub, SIGNAL(published()), SLOT(pub_published()));
		connect(&pub, SIGNAL(error(XMPP::ServiceLocalPublisher::Error)),
			SLOT(pub_error(XMPP::ServiceLocalPublisher::Error)));

		pub.publish(instance, type, port, attribs);
	}

signals:
	void quit();

private slots:
	void pub_published()
	{
		printf("Published\n");
		if(!extra_null.isEmpty())
		{
			NameRecord rec;
			rec.setNull(extra_null);
			pub.addRecord(rec);
		}
	}

	void pub_error(XMPP::ServiceLocalPublisher::Error e)
	{
		printf("Error: [%d]\n", e);
		emit quit();
	}
};

class StunBind : public QObject
{
	Q_OBJECT
public:
	QHostAddress addr;
	int port;
	int localPort;
	QUdpSocket *sock;
	StunTransactionPool *pool;
	StunBinding *binding;

	~StunBind()
	{
		// make sure transactions are always deleted before the pool
		delete binding;
	}

public slots:
	void start()
	{
		sock = new QUdpSocket(this);
		connect(sock, SIGNAL(readyRead()), SLOT(sock_readyRead()));

		pool = new StunTransactionPool(StunTransaction::Udp, this);
		connect(pool, SIGNAL(outgoingMessage(const QByteArray &, const QHostAddress &, int)), SLOT(pool_outgoingMessage(const QByteArray &, const QHostAddress &, int)));

		if(!sock->bind(localPort != -1 ? localPort : 0))
		{
			printf("Error binding to local port.\n");
			emit quit();
			return;
		}

		printf("Bound to local port %d.\n", sock->localPort());

		binding = new StunBinding(pool);
		connect(binding, SIGNAL(success()), SLOT(binding_success()));
		connect(binding, SIGNAL(error(XMPP::StunBinding::Error)), SLOT(binding_error(XMPP::StunBinding::Error)));
		binding->start();
	}

signals:
	void quit();

private slots:
	void sock_readyRead()
	{
		while(sock->hasPendingDatagrams())
		{
			QByteArray buf(sock->pendingDatagramSize(), 0);
			QHostAddress from;
			quint16 fromPort;

			sock->readDatagram(buf.data(), buf.size(), &from, &fromPort);
			if(from == addr && fromPort == port)
			{
				processDatagram(buf);
			}
			else
			{
				printf("Response from unknown sender %s:%d, dropping.\n", qPrintable(from.toString()), fromPort);
			}
		}
	}

	void pool_outgoingMessage(const QByteArray &packet, const QHostAddress &toAddress, int toPort)
	{
		// in this example, we aren't using IP-associated transactions
		Q_UNUSED(toAddress);
		Q_UNUSED(toPort);

		sock->writeDatagram(packet, addr, port);
	}

	void binding_success()
	{
		QHostAddress saddr = binding->reflexiveAddress();
		quint16 sport = binding->reflexivePort();
		printf("Server says we are %s;%d\n", qPrintable(saddr.toString()), sport);
		emit quit();
	}

	void binding_error(XMPP::StunBinding::Error e)
	{
		Q_UNUSED(e);
		printf("Error: %s\n", qPrintable(binding->errorString()));
		emit quit();
	}

private:
	void processDatagram(const QByteArray &buf)
	{
		StunMessage message = StunMessage::fromBinary(buf);
		if(message.isNull())
		{
			printf("Warning: server responded with what doesn't seem to be a STUN packet, skipping.\n");
			return;
		}

		if(!pool->writeIncomingMessage(message))
			printf("Warning: received unexpected message, skipping.\n");
	}
};

class TurnEcho : public QObject
{
	Q_OBJECT
public:
	int mode;
	QHostAddress relayAddr;
	int relayPort;
	QString relayUser, relayPass, relayRealm;
	QHostAddress peerAddr;
	int peerPort;
	QUdpSocket *udp;
	QTcpSocket *tcp;
	QCA::TLS *tls;
	QByteArray inStream; // incoming stream
	StunTransactionPool *pool;
	StunAllocate *allocate;

	TurnEcho() :
		udp(0),
		tcp(0),
		tls(0),
		allocate(0)
	{
	}

	~TurnEcho()
	{
		// make sure transactions are always deleted before the pool
		delete allocate;
	}

public slots:
	void start()
	{
		StunTransaction::Mode poolMode;

		if(mode == 0)
		{
			poolMode = StunTransaction::Udp;

			udp = new QUdpSocket(this);
			connect(udp, SIGNAL(readyRead()), SLOT(udp_readyRead()));
		}
		else if(mode == 1 || mode == 2)
		{
			poolMode = StunTransaction::Tcp;

			tcp = new QTcpSocket(this);
			connect(tcp, SIGNAL(connected()), SLOT(tcp_connected()));
			connect(tcp, SIGNAL(readyRead()), SLOT(tcp_readyRead()));
			connect(tcp, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(tcp_error(QAbstractSocket::SocketError)));

			if(mode == 2)
			{
				tls = new QCA::TLS(this);
				connect(tls, SIGNAL(handshaken()), SLOT(tls_handshaken()));
				connect(tls, SIGNAL(readyRead()), SLOT(tls_readyRead()));
				connect(tls, SIGNAL(readyReadOutgoing()), SLOT(tls_readyReadOutgoing()));
				connect(tls, SIGNAL(error()), SLOT(tls_error()));
			}
		}

		pool = new StunTransactionPool(poolMode, this);
		connect(pool, SIGNAL(outgoingMessage(const QByteArray &, const QHostAddress &, int)), SLOT(pool_outgoingMessage(const QByteArray &, const QHostAddress &, int)));
		connect(pool, SIGNAL(needAuthParams()), SLOT(pool_needAuthParams()));

		pool->setLongTermAuthEnabled(true);
		if(!relayUser.isEmpty())
		{
			pool->setUsername(relayUser);
			pool->setPassword(relayPass.toUtf8());
			if(!relayRealm.isEmpty())
				pool->setRealm(relayRealm);
		}

		if(udp)
		{
			if(!udp->bind())
			{
				printf("Error binding to local port.\n");
				emit quit();
				return;
			}

			doAllocate();
		}
		else
		{
			printf("TCP connecting...\n");
			tcp->connectToHost(relayAddr, relayPort);
		}
	}

signals:
	void quit();

private:
	void doAllocate()
	{
		allocate = new StunAllocate(pool);
		connect(allocate, SIGNAL(started()), SLOT(allocate_started()));
		connect(allocate, SIGNAL(stopped()), SLOT(allocate_stopped()));
		connect(allocate, SIGNAL(error(XMPP::StunAllocate::Error)), SLOT(allocate_error(XMPP::StunAllocate::Error)));
		connect(allocate, SIGNAL(permissionsChanged()), SLOT(allocate_permissionsChanged()));
		connect(allocate, SIGNAL(channelsChanged()), SLOT(allocate_channelsChanged()));

		allocate->setClientSoftwareNameAndVersion("nettool (Iris)");

		printf("Allocating...\n");
		allocate->start();
	}

	void sendTestPacket()
	{
		QByteArray buf = "Hello, world!";
		QByteArray packet = allocate->encode(buf, peerAddr, peerPort);

		printf("Relaying test packet of %d bytes (<=%d overhead) to %s;%d...\n", packet.size(), allocate->packetHeaderOverhead(peerAddr, peerPort), qPrintable(peerAddr.toString()), peerPort);

		if(udp)
			udp->writeDatagram(packet, relayAddr, relayPort);
		else
		{
			if(tls)
				tls->write(packet);
			else
				tcp->write(packet);
		}
	}

private slots:
	void udp_readyRead()
	{
		while(udp->hasPendingDatagrams())
		{
			QByteArray buf(udp->pendingDatagramSize(), 0);
			QHostAddress from;
			quint16 fromPort;

			udp->readDatagram(buf.data(), buf.size(), &from, &fromPort);
			if(from == relayAddr && fromPort == relayPort)
			{
				processDatagram(buf);
			}
			else
			{
				printf("Response from unknown sender %s:%d, dropping.\n", qPrintable(from.toString()), fromPort);
			}
		}
	}

	void tcp_connected()
	{
		printf("TCP connected\n");

		if(tls)
		{
			printf("TLS handshaking...\n");
			tls->startClient();
		}
		else
			doAllocate();
	}

	void tcp_readyRead()
	{
		QByteArray buf = tcp->readAll();

		if(tls)
			tls->writeIncoming(buf);
		else
			processStream(buf);
	}

	void tcp_error(QAbstractSocket::SocketError e)
	{
		Q_UNUSED(e);
		printf("TCP error.\n");
		emit quit();
	}

	void tls_handshaken()
	{
		printf("TLS handshake completed\n");

		tls->continueAfterStep();

		doAllocate();
	}

	void tls_readyRead()
	{
		processStream(tls->read());
	}

	void tls_readyReadOutgoing()
	{
		tcp->write(tls->readOutgoing());
	}

	void tls_error()
	{
		printf("TLS error.\n");
		emit quit();
	}

	void pool_outgoingMessage(const QByteArray &packet, const QHostAddress &toAddress, int toPort)
	{
		// in this example, we aren't using IP-associated transactions
		Q_UNUSED(toAddress);
		Q_UNUSED(toPort);

		if(udp)
			udp->writeDatagram(packet, relayAddr, relayPort);
		else
		{
			if(tls)
				tls->write(packet);
			else
				tcp->write(packet);
		}
	}

	void pool_needAuthParams()
	{
		relayUser = prompt("Username:");
		relayPass = prompt("Password:");

		pool->setUsername(relayUser);
		pool->setPassword(relayPass.toUtf8());

		QString str = prompt(QString("Realm: [%1]").arg(pool->realm()));
		if(!str.isEmpty())
		{
			relayRealm = str;
			pool->setRealm(relayRealm);
		}
		else
			relayRealm = pool->realm();

		pool->continueAfterParams();
	}

	void allocate_started()
	{
		printf("Allocate started\n");
		QHostAddress saddr = allocate->reflexiveAddress();
		quint16 sport = allocate->reflexivePort();
		printf("Server says we are %s;%d\n", qPrintable(saddr.toString()), sport);
		saddr = allocate->relayedAddress();
		sport = allocate->relayedPort();
		printf("Server relays via %s;%d\n", qPrintable(saddr.toString()), sport);

		printf("Setting permission for peer address %s\n", qPrintable(peerAddr.toString()));
		QList<QHostAddress> perms;
		perms += peerAddr;
		allocate->setPermissions(perms);
	}

	void allocate_stopped()
	{
		printf("Done\n");
		emit quit();
	}

	void allocate_error(XMPP::StunAllocate::Error e)
	{
		Q_UNUSED(e);
		printf("Error: %s\n", qPrintable(allocate->errorString()));
		emit quit();
	}

	void allocate_permissionsChanged()
	{
		printf("PermissionsChanged\n");

		printf("Setting channel for peer address/port %s;%d\n", qPrintable(peerAddr.toString()), peerPort);
		QList<StunAllocate::Channel> channels;
		channels += StunAllocate::Channel(peerAddr, peerPort);
		allocate->setChannels(channels);
	}

	void allocate_channelsChanged()
	{
		printf("ChannelsChanged\n");

		sendTestPacket();
	}

private:
	void processDatagram(const QByteArray &buf)
	{
		QByteArray data;
		QHostAddress fromAddr;
		int fromPort;

		bool notStun;
		if(!pool->writeIncomingMessage(buf, &notStun))
		{
			if(notStun)
			{
				// not stun?  maybe it is a data packet
				data = allocate->decode(buf, &fromAddr, &fromPort);
				if(!data.isNull())
				{
					printf("Received ChannelData-based data packet\n");
					processDataPacket(data, fromAddr, fromPort);
					return;
				}
			}
			else
			{
				// packet might be stun not owned by pool.
				//   let's see
				StunMessage message = StunMessage::fromBinary(buf);
				if(!message.isNull())
				{
					data = allocate->decode(message, &fromAddr, &fromPort);

					if(!data.isNull())
					{
						printf("Received STUN-based data packet\n");
						processDataPacket(data, fromAddr, fromPort);
					}
					else
						printf("Warning: server responded with an unexpected STUN packet, skipping.\n");

					return;
				}
			}

			printf("Warning: server responded with what doesn't seem to be a STUN or data packet, skipping.\n");
		}
	}

	void processStream(const QByteArray &in)
	{
		inStream += in;

		while(1)
		{
			QByteArray packet;

			// try to extract ChannelData or a STUN message from
			//   the stream
			packet = StunAllocate::readChannelData((const quint8 *)inStream.data(), inStream.size());
			if(packet.isNull())
			{
				packet = StunMessage::readStun((const quint8 *)inStream.data(), inStream.size());
				if(packet.isNull())
					break;
			}

			inStream = inStream.mid(packet.size());
			processDatagram(in);
		}
	}

	void processDataPacket(const QByteArray &buf, const QHostAddress &addr, int port)
	{
		printf("Received %d bytes from %s:%d: [%s]\n", buf.size(), qPrintable(addr.toString()), port, buf.data());

		printf("Deallocating...\n");
		allocate->stop();
	}
};

void usage()
{
	printf("nettool: simple testing utility\n");
	printf("usage: nettool (options) [command]\n");
	printf("  options: --user=x, --pass=x, --realm=x\n");
	printf("\n");
	printf(" netmon                                            monitor network interfaces\n");
	printf(" rname (-r) [domain] (record type)                 look up record (default = a)\n");
	printf(" rnamel [domain] [record type]                     look up record (long-lived)\n");
	printf(" browse [service type]                             browse for local services\n");
	printf(" rservi [instance] [service type]                  look up browsed instance\n");
	printf(" rservd [domain] [service type]                    look up normal SRV\n");
	printf(" rservp [domain] [port]                            look up non-SRV\n");
	printf(" pserv [inst] [type] [port] (attr) (-a [rec])      publish service instance\n");
	printf(" stun [addr](;port) (local port)                   STUN binding\n");
	printf(" turn [mode] [relayaddr](;port) [peeraddr](;port)  TURN UDP echo test\n");
	printf("\n");
	printf("record types: a aaaa ptr srv mx txt hinfo null\n");
	printf("service types: _service._proto format (e.g. \"_xmpp-client._tcp\")\n");
	printf("attributes: var0[=val0],...,varn[=valn]\n");
	printf("rname -r: for null type, dump raw record data to stdout\n");
	printf("pserv -a: add extra record.  format: null:filename.dat\n");
	printf("turn modes: udp tcp tcp-tls\n");
	printf("\n");
}

int main(int argc, char **argv)
{
	QCA::Initializer qcaInit;
	QCoreApplication qapp(argc, argv);

	QStringList args = qapp.arguments();
	args.removeFirst();

	QString user, pass, realm;

	for(int n = 0; n < args.count(); ++n)
	{
		QString s = args[n];
		if(!s.startsWith("--"))
			continue;
		QString var;
		QString val;
		int x = s.indexOf('=');
		if(x != -1)
		{
			var = s.mid(2, x - 2);
			val = s.mid(x + 1);
		}
		else
		{
			var = s.mid(2);
		}

		bool known = true;

		if(var == "user")
			user = val;
		else if(var == "pass")
			pass = val;
		else if(var == "realm")
			realm = val;
		else
			known = false;

		if(!known)
		{
			fprintf(stderr, "Unknown option '%s'.\n", qPrintable(var));
			return 1;
		}

		args.removeAt(n);
		--n; // adjust position
	}

	if(args.isEmpty())
	{
		usage();
		return 1;
	}

	if(args[0] == "netmon")
	{
		NetMonitor a;
		QObject::connect(&a, SIGNAL(quit()), &qapp, SLOT(quit()));
		QTimer::singleShot(0, &a, SLOT(start()));
		qapp.exec();
	}
	else if(args[0] == "rname" || args[0] == "rnamel")
	{
		bool null_dump = false;
		for(int n = 1; n < args.count(); ++n)
		{
			if(args[n] == "-r")
			{
				null_dump = true;
				args.removeAt(n);
				--n;
			}
		}

		if(args.count() < 2)
		{
			usage();
			return 1;
		}
		if(args[0] == "rnamel" && args.count() < 3)
		{
			usage();
			return 1;
		}
		int x = NameRecord::A;
		if(args.count() >= 3)
		{
			x = str2rtype(args[2]);
			if(x == -1)
			{
				usage();
				return 1;
			}
		}
		ResolveName a;
		a.name = args[1];
		a.type = (NameRecord::Type)x;
		a.longlived = (args[0] == "rnamel") ? true : false;
		if(args[0] == "rname" && null_dump)
			a.null_dump = true;
		QObject::connect(&a, SIGNAL(quit()), &qapp, SLOT(quit()));
		QTimer::singleShot(0, &a, SLOT(start()));
		qapp.exec();
	}
	else if(args[0] == "browse")
	{
		if(args.count() < 2)
		{
			usage();
			return 1;
		}

		BrowseServices a;
		a.type = args[1];
		QObject::connect(&a, SIGNAL(quit()), &qapp, SLOT(quit()));
		QTimer::singleShot(0, &a, SLOT(start()));
		qapp.exec();
	}
	else if(args[0] == "rservi" || args[0] == "rservd" || args[0] == "rservp")
	{
		// they all take 2 params
		if(args.count() < 3)
		{
			usage();
			return 1;
		}

		ResolveService a;
		if(args[0] == "rservi")
		{
			a.mode = 0;
			a.instance = args[1];
			a.type = args[2];
		}
		else if(args[0] == "rservd")
		{
			a.mode = 1;
			a.domain = args[1];
			a.type = args[2];
		}
		else // rservp
		{
			a.mode = 2;
			a.domain = args[1];
			a.port = args[2].toInt();
		}
		QObject::connect(&a, SIGNAL(quit()), &qapp, SLOT(quit()));
		QTimer::singleShot(0, &a, SLOT(start()));
		qapp.exec();
	}
	else if(args[0] == "pserv")
	{
		QStringList addrecs;
		for(int n = 1; n < args.count(); ++n)
		{
			if(args[n] == "-a")
			{
				if(n + 1 < args.count())
				{
					addrecs += args[n + 1];
					args.removeAt(n);
					args.removeAt(n);
					--n;
				}
				else
				{
					usage();
					return 1;
				}
			}
		}

		QByteArray extra_null;
		for(int n = 0; n < addrecs.count(); ++n)
		{
			const QString &str = addrecs[n];
			int x = str.indexOf(':');
			if(x == -1 || str.mid(0, x) != "null")
			{
				usage();
				return 1;
			}

			QString null_file = str.mid(x + 1);

			if(!null_file.isEmpty())
			{
				QFile f(null_file);
				if(!f.open(QFile::ReadOnly))
				{
					printf("can't read file\n");
					return 1;
				}
				extra_null = f.readAll();
			}
		}

		if(args.count() < 4)
		{
			usage();
			return 1;
		}

		QMap<QString,QByteArray> attribs;
		if(args.count() > 4)
		{
			QStringList parts = args[4].split(',');
			for(int n = 0; n < parts.count(); ++n)
			{
				const QString &str = parts[n];
				int x = str.indexOf('=');
				if(x != -1)
					attribs.insert(str.mid(0, x), str.mid(x + 1).toUtf8());
				else
					attribs.insert(str, QByteArray());
			}
		}

		PublishService a;
		a.instance = args[1];
		a.type = args[2];
		a.port = args[3].toInt();
		a.attribs = attribs;
		a.extra_null = extra_null;
		QObject::connect(&a, SIGNAL(quit()), &qapp, SLOT(quit()));
		QTimer::singleShot(0, &a, SLOT(start()));
		qapp.exec();
	}
	else if(args[0] == "stun")
	{
		if(args.count() < 2)
		{
			usage();
			return 1;
		}

		QString addrstr, portstr;
		int x = args[1].indexOf(';');
		if(x != -1)
		{
			addrstr = args[1].mid(0, x);
			portstr = args[1].mid(x + 1);
		}
		else
			addrstr = args[1];

		QHostAddress addr = QHostAddress(addrstr);
		if(addr.isNull())
		{
			printf("Error: addr must be an IP address\n");
			return 1;
		}

		int port = 3478;
		if(!portstr.isEmpty())
			port = portstr.toInt();

		int localPort = -1;
		if(args.count() >= 3)
			localPort = args[2].toInt();

		if(!QCA::isSupported("hmac(sha1)"))
		{
			printf("Error: Need hmac(sha1) support to use STUN.\n");
			return 1;
		}

		StunBind a;
		a.localPort = localPort;
		a.addr = addr;
		a.port = port;
		QObject::connect(&a, SIGNAL(quit()), &qapp, SLOT(quit()));
		QTimer::singleShot(0, &a, SLOT(start()));
		qapp.exec();
	}
	else if(args[0] == "turn")
	{
		if(args.count() < 4)
		{
			usage();
			return 1;
		}

		int mode;
		if(args[1] == "udp")
			mode = 0;
		else if(args[1] == "tcp")
			mode = 1;
		else if(args[1] == "tcp-tls")
			mode = 2;
		else
		{
			usage();
			return 1;
		}

		QString addrstr, portstr;
		int x = args[2].indexOf(';');
		if(x != -1)
		{
			addrstr = args[2].mid(0, x);
			portstr = args[2].mid(x + 1);
		}
		else
			addrstr = args[2];

		QHostAddress raddr = QHostAddress(addrstr);
		if(raddr.isNull())
		{
			printf("Error: relayaddr must be an IP address\n");
			return 1;
		}

		int rport = 3478;
		if(!portstr.isEmpty())
			rport = portstr.toInt();

		portstr.clear();
		x = args[3].indexOf(';');
		if(x != -1)
		{
			addrstr = args[3].mid(0, x);
			portstr = args[3].mid(x + 1);
		}
		else
			addrstr = args[3];

		QHostAddress paddr = QHostAddress(addrstr);
		if(raddr.isNull())
		{
			printf("Error: peeraddr must be an IP address\n");
			return 1;
		}

		int pport = 4588;
		if(!portstr.isEmpty())
			pport = portstr.toInt();

		if(!QCA::isSupported("hmac(sha1)"))
		{
			printf("Error: Need hmac(sha1) support to use TURN.\n");
			return 1;
		}

		if(mode == 2 && !QCA::isSupported("tls"))
		{
			printf("Error: Need tls support to use tcp-tls mode.\n");
			return 1;
		}

		TurnEcho a;
		a.mode = mode;
		a.relayAddr = raddr;
		a.relayPort = rport;
		a.relayUser = user;
		a.relayPass = pass;
		a.relayRealm = realm;
		a.peerAddr = paddr;
		a.peerPort = pport;
		QObject::connect(&a, SIGNAL(quit()), &qapp, SLOT(quit()));
		QTimer::singleShot(0, &a, SLOT(start()));
		qapp.exec();
	}
	else
	{
		usage();
		return 1;
	}
	return 0;
}

#include "main.moc"