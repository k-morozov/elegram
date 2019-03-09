#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include "src/model/chatslist.h"
#include "src/model/messageslist.h"
#include "src/model/contactslist.h"
#include "src/network/network_session.h"

namespace elegram {
  class Profile {
   public:
    Profile(quint64 userId, QString &&userName, QString &&userEmail);

    quint64 userId() const;
    QString userName() const;
    QString userEmail() const;

   private:
    quint64 m_userId;
    QString m_userName;
    QString m_userEmail;
  };

  /**
   * Responds to requests from BackendFacade using QT signal mechanism.
   */
  class Backend : public QObject {
   Q_OBJECT
   public:
    Backend(ChatsList *chatsList, ContactsList *contactsList, MessagesList *mesgList, const QString &serverIp,
            const QString &serverPort, QObject *parent = Q_NULLPTR);

    void signin(const QString &email, const QString &password);
    void signup(const QString &name, const QString &email, const QString &password);
    void sendMessage(quint64 chatId, const QString &message);
    void addContact(const QString &email);
    void loadChats();
    void loadContacts();
    void loadData();
    void loadMessages(quint64 chatId);
    bool updateConnectionSettings(const QString &ip, const QString &port);
    void createChat(quint64 friendId);

    // setters
    void setProfile(std::unique_ptr<Profile> &&profile);

    // getters
    ChatsList *chatsList() const;
    ContactsList *contactsList() const;
    MessagesList *mesagesList() const;
    Session &connection() const;

    quint64 userId() const;
    QString userName() const;
    QString userEmail() const;

   signals:
    void loggedIn(bool);
    void registered(bool);
    void addedContact(bool);
    void loadedChats();
    void loadedContacts();
    void loadedMessages();
    void loadedData();
    void sendedMessage(bool);
    void updatedConnectionSettings(const QString &);
    void operationAborted(const QString &errorMessage);
    void showMessage(const QString &message);
    void chatCreated(bool isCreated);
    void disconnected();

   private:
    std::unique_ptr<Session> m_session;
    std::unique_ptr<Profile> m_profile = nullptr;
    ChatsList *m_chatsList;
    ContactsList *m_contactsList;
    MessagesList *m_mesagesList;
  };
} // namespace elegram

#endif // BACKEND_H
