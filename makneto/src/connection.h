/*
 * connection.h
 *
 * Copyright (C) 2007 Jaroslav Reznik <rezzabuh@gmail.com>
 */

#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>

#include <xmpp_status.h>
#include <xmpp_rosterx.h>
#include <xmpp_jid.h>
#include <xmpp_task.h>
#include <QTextStream>
/**
 * This is main connection class for Makneto
 *
 * @short Connection class
 * @author Jaroslav Reznik <rezzabuh@gmail.com>
 * @version 0.1
 */

namespace XMPP 
{
	class Client;
	class Jid;
	class ClientStream;
	class AdvancedConnector;
	class QCATLSHandler;
	class RosterItem;
	class Resource;
	class Message;
	class FileTransfer;
	class Status;
}

namespace QCA 
{
	class TLS;
}

using namespace XMPP;

class Makneto;
class QTextStream;

class Connection : public QObject
{
	Q_OBJECT
public:
	/**
	* Default constructor
	*/
	Connection(Makneto *makneto);

	/**
	* Destructor
	*/
	virtual ~Connection();

	bool login();
	void clientDisconnect();
	bool logout();
	void setStatus(Status);
	bool isOnline();
  
  Jid jid() { return m_jid; }
  
  void setRole(const Jid &jid, const QString &nick, const QString &role, const QString &reason = QString());
  void setAffiliation(const Jid &jid, const Jid &userJid, const QString &affiliation, const QString &reason = QString());
  QString genUniqueId();

  /*! \brief Get Iris client connection */
  Client * client() const;
  /*! \brief Get root Task object to create new requests. */
  Task * rootTask() const;

private slots:
	void connected();
	void needAuthParams(bool, bool, bool);
	void warning(int code);
	void error(int code);
	void authenticated();
	void connectionClosed();
	void tlsHandshaken();
	void sessionStart_finished();

	void client_rosterRequestFinished(bool, int, const QString &);
	void client_rosterItemAdded(const RosterItem &);
	void client_rosterItemUpdated(const RosterItem &);
	void client_rosterItemRemoved(const RosterItem &);
	void client_resourceAvailable(const Jid &, const Resource &);
	void client_resourceUnavailable(const Jid &, const Resource &);
	void client_presenceError(const Jid &, int, const QString &);
	void client_messageReceived(const Message &message);
	void client_subscription(const Jid &, const QString &, const QString&);
	void client_debugText(const QString &);
	void client_xmlIncoming(const QString &);
	void client_xmlOutgoing(const QString &);
	void client_incomingFileTransfer();
  void client_groupChatJoined(const Jid &);
  void client_groupChatLeft(const Jid &);
  void client_groupChatPresence(const Jid &, const Status &);
  void client_groupChatError(const Jid &, int, const QString &);

	void sendMessage(const Message &);
public slots:
	void addUser(const Jid &jid, const QString &group, bool requestAuth);
  
  /** Sends XML message to the Jabber server **/
  void send(const QDomElement &);
  void send(const QString &);
  
  // GroupChat
  /** Join to the groupchat
   * @p host Host of the conference room (e.g. conf.netlab.cz)
   * @p room Room name (e.g. linux)
   * @p nick Your nick in the room
   * @p password Password for the conference
   * @p maxchars Chars of history returned after connection
   * @p maxstanzas Lines of history returned after connection
   * @p seconds Seconds of history returned after connection
   * @p status Your status
   **/
  bool groupChatJoin(const QString &host, const QString &room, const QString &nick, const QString& password = QString(), 
                     int maxchars = -1, int maxstanzas = -1, int seconds = -1, const Status& = Status());
  
  /** Disconnect from the groupchat
   * @p host Host of the conference room (e.g. conf.netlab.cz)
   * @p room Room name (e.g. linux)
   **/
  void groupChatLeave(const Jid &jid);
  
  /** Granting voice to visitor by moderator
   * @p jid Jabber ID of the conference room
   * @p nick Nick of user that will have the voice
   * @p reason Reason of granting the voice
   **/
  void groupChatGrantVoice(const Jid &jid, const QString &nick, const QString &reason = QString());
  
  /** Revoking voice from participant by moderator
   * @p jid Jabber ID of the conference room
   * @p nick Nick of user that the voice will be revoked from
   * @p reason Reason of revoking the voice
   **/
  void groupChatRevokeVoice(const Jid &jid, const QString &nick, const QString &reason = QString());
  
  /** Requesting the voice
   * @p jid Jabber ID of the room
   **/
  void groupChatRequestVoice(const Jid &jid);
  
  /** Change subject of the chatroom
   * @p jid Jabber ID of the conference room
   * @p subject New subject for the room
   **/
  void groupChatSubjectChange(const Jid &jid, const QString &subject);
  
  /** Kick user from chat room
   * @p jid Jabber ID of the conference room
   * @p nick Nick of kicked user
   * @p reason Reason of kicking user
   **/
  void groupChatKickUser(const Jid &jid, const QString &nick, const QString &reason = QString());
  
  /** Ban user from chat room
   * @p jid Jabber ID of the conference room
   * @p nick Nick of banned user
   * @p reason Reason of banning user
   **/
  void groupChatBanUser(const Jid &jid, const Jid &userJid, const QString &reason = QString());
  
  /** Granting membership in room
   * @p jid Jabber ID of the conference room
   * @p userJid Jabber ID of user that will have the membership
   * @p reason Reason of granting the membership
   **/
  void groupChatGrantMembership(const Jid &jid, const Jid &userJid, const QString &reason = QString());
  
  /** Revoking membership in room
   * @p jid Jabber ID of the conference room
   * @p userJid Jabber ID of user that will lose the membership
   * @p reason Reason of revoking the membership
   **/
  void groupChatRevokeMembership(const Jid &jid, const Jid &userJid, const QString &reason = QString());
  
  /** Granting moderation to the user
   * @p jid Jabber ID of the conference room
   * @p nick Nick of user that will become a moderator
   * @p reason Reason of granting
   **/
  void groupChatGrantModeration(const Jid &jid, const QString &nick, const QString &reason = QString());
  
  /** Revoking moderation from the user
   * @p jid Jabber ID of the conference room
   * @p nick Nick of user that stopped to be a moderator
   * @p reason Reason of revoking
   **/
  void groupChatRevokeModeration(const Jid &jid, const QString &nick, const QString &reason = QString());
  
  /** Granting administration to the user
   * @p jid Jabber ID of the conference room
   * @p nick Nick of user that will become an admin
   * @p reason Reason of granting
   **/
  void groupChatGrantAdministration(const Jid &jid, const Jid &userJid, const QString &reason = QString());
  
  /** Revoking administration from the user
   * @p jid Jabber ID of the conference room
   * @p nick Nick of user that stopped to be an admin
   * @p reason Reason of revoking
   **/
  void groupChatRevokeAdministration(const Jid &jid, const Jid &userJid, const QString &reason = QString());
  
    /** Granting ownership to the user
   * @p jid Jabber ID of the conference room
   * @p nick Nick of user that will become a owner
   * @p reason Reason of granting
     **/
  void groupChatGrantOwnership(const Jid &jid, const Jid &userJid, const QString &reason = QString());
  
  /** Revoking ownership from the user
   * @p jid Jabber ID of the conference room
   * @p nick Nick of user that stopped to be a owner
   * @p reason Reason of revoking
   **/
  void groupChatRevokeOwnership(const Jid &jid, const Jid &userJid, const QString &reason = QString());

  
signals:
	void connMessageReceived(const Message &);
	void connConnected(void);
	void connDisconnected(void);
	void connIncomingFileTransfer(FileTransfer *);
	void connStatusChanged(const XMPP::Status &);
  
  // GroupChat
  /** You are successfully connected to the MUC
   * @p jid Jabber ID of chatroom you are connected to
   **/
  void groupChatJoined(const Jid &);
  
  /** You have left chatroom
   * @p jid Jabber ID of left chatroom
   **/
  void groupChatLeft(const Jid &);
  
  /** Presence of some user in room changed
   * @p jid Jabber ID of the conference room
   * @p status Status changed to this one
   **/
  void groupChatPresence(const Jid &, const Status &);
  
  /** Some error occured
   * @p jid Jabber ID of the room that error occured in
   * @p i
   * @p text Text of error message
   **/
  void groupChatError(const Jid &, int, const QString &);
  
  /** Information for moderator of successfull voice granting
   * @p jid Jabber ID of the user the voice has been granted to
   **/
//   void groupChatVoiceGrantSucceeded(const Jid &jid);
  
  /** Information for all users of granting voice
   * @p jid Jabber ID of the chatroom
   * @p nick Nickname of user that voice has been granted to
   * @p reason Reason of granting voice
   **/
  void groupChatVoiceGranted(const Jid &jid, const QString &nick, const QString &reason = QString());
  
  /** Information for moderator of successfull voice revoking
   * @p jid Jabber ID of the user the voice has been revoked from
   **/
//   void groupChatVoiceRevokeSucceeded(const Jid &jid);
  
  /** Information for all users of revoking voice
   * @p jid Jabber ID of the chatroom
   * @p nick Nickname of user that voice has been revoked from
   * @p reason Reason of granting voice
   **/
  void groupChatVoiceRevoked(const Jid &jid, const QString &nick, const QString &reason = QString());
  
  /** Information for moderator of requesting the voice by user
   * @p jid Jabber ID of the room
   * @p nick Nick of user that is requesting the voice
   **/
  void groupChatVoiceRequested(const Jid &jid, const QString &nick);
  
  /** Information about changing subject in the room
   * @p jid Jabber ID of the room
   * @p subject New subject in the room
   **/
  void groupChatSubjectChanged(const Jid &jid, const QString &subject);
  
  /** User has been kick from chatroom
   * @p jid Jabber ID of the chatroom
   * @p nick Kicked user's nick
   * @p reason Reason why is user kicked
   * @p actor User who kicked him
   **/
  void groupChatUserKicked(const Jid &jid, const QString &nick, const QString &reason, const QString &actor);
  
  /** Information for moderator about successfull kicking
   * @p jid Jabber ID of kicked user
   **/
//   void groupChatKickSucceeded(const Jid &jid);
  
  /** User has been banned in the chatroom
   * @p jid Jabber ID of the chatroom
   * @p nick Banned user's nick
   * @p reason Reason why is user banned
   * @p actor User who banned him
   **/
  void groupChatUserBanned(const Jid &jid, const QString &nick, const QString &reason, const QString &actor);
  
  /** User has been removed from the chatroom by removing him from member list (occures only in member-only room)
   * @p jid Jabber ID of the chatroom
   * @p nick Removed user's nick
   **/
  void groupChatUserRemovedAsNonMember(const Jid &jid, const QString &nick);
  
  /** User has become a member of the chatroom
   * @p jid Jabber ID of the chatroom
   * @p userJid Jabber ID of new member
   * @p reason Reason
   **/
  void groupChatMembershipGranted(const Jid &jid, const Jid &userJid, const QString &reason = QString());

    /** User membership has been revoked
   * @p jid Jabber ID of the chatroom
   * @p userJid Jabber ID of user whose membership has been revoked
   * @p reason Reason
     **/
  void groupChatMembershipRevoked(const Jid &jid, const Jid &userJid, const QString &reason = QString());
  
  /** Information for all users of becaming user the moderator
   * @p jid Jabber ID of the chatroom
   * @p nick Nickname of user that became a moderator
   * @p reason Reason of granting voice
   **/
  void groupChatModerationGranted(const Jid &jid, const QString &nick, const QString &reason = QString());
  
  /** Information for all users of revoking moderation
   * @p jid Jabber ID of the chatroom
   * @p nick Nickname of user that stopped to be a moderator
   * @p reason Reason of granting voice
   **/
  void groupChatModerationRevoked(const Jid &jid, const QString &nick, const QString &reason = QString());

  
private:
	Client *m_client;
	ClientStream *m_stream;
	AdvancedConnector *m_conn;
	QCATLSHandler *m_tlsHandler;
	Jid m_jid;
	QCA::TLS *m_tls;
	Makneto *m_makneto;
	bool m_rosterFinished;
  
  // TODO: Remove logging when not necessary
  QTextStream log;

protected:
	void sessionStarted();
};

#endif // CONNECTION_H
