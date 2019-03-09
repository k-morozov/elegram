#ifndef BACKEND_FACADE_H
#define BACKEND_FACADE_H

#include <QObject>
#include <QString>
#include <memory>
#include "backend.h"

namespace elegram {
  class ChatsList;
  class ContactsList;
  class MessagesList;
  //  class Backend;

  /**
   * @brief The Facade class, qml code know it as "backend".
   * This is mediator between qml code and real Backend.
   */
  class BackEndFacade : public QObject {
   Q_OBJECT
    Q_PROPERTY(QString serverIp
                   READ
                   serverIp
                   WRITE
                   setServerIp
                   NOTIFY
                   serverIpChanged)
    Q_PROPERTY(QString serverPort
                   READ
                   serverPort
                   WRITE
                   setServerPort
                   NOTIFY
                   serverPortChanged)

    Q_PROPERTY(quint64 userId
                   READ
                   userId  /* WRITE setUserId NOTIFY userIdChanged */ )
    Q_PROPERTY(QString userName
                   READ
                   userName /* WRITE setUserName NOTIFY userNameChanged */ )
    Q_PROPERTY(QString userEmail
                   READ
                   userEmail /* WRITE setUserEmail NOTIFY userEmailChanged */ )

   public:
    BackEndFacade(ChatsList *chatsList, ContactsList *contactsList, MessagesList *mesgList,
                  QObject *parent = Q_NULLPTR);

    Q_INVOKABLE bool availible();

    /* Button callbacks -- backend API */
    Q_INVOKABLE void signin(const QString &email, const QString &password);
    Q_INVOKABLE void signup(const QString &name, const QString &email, const QString &password);
    Q_INVOKABLE void sendMessage(quint64 chatId, const QString &message);
    Q_INVOKABLE void addContact(const QString &email);
    Q_INVOKABLE void loadChats();
    Q_INVOKABLE void loadContacts();
    Q_INVOKABLE void loadData();
    Q_INVOKABLE void loadMessages(quint64 chatId);
    Q_INVOKABLE QString get_contact_name(quint64 sender_id);
    Q_INVOKABLE void updateConnectionSettings(const QString &ip, const QString &port);
    Q_INVOKABLE void createChat(quint64 friendId);

    QString serverIp() const;
    void setServerIp(const QString &serverIp);

    QString serverPort() const;
    void setServerPort(const QString &port);

    quint64 userId() const;
    QString userName() const;
    QString userEmail() const;

   private:
    void setAvailible(bool availible);

   signals: // send to qml
    void doLoginReaction(bool isLogged);
    void doRegisterReaction(bool isRegistered);
    void doLoadChatsReaction();
    void doLoadContactsReaction();
    void doLoadMessagesReaction();
    void doSendMessageReaction(bool status);
    void doAddContactReaction(bool isAdded);
    void doLoadDataReaction();
    void doUpdateConnectionSettingsReaction(const QString &errorMesg);
    void doOperationAborted(const QString &errorMessage);
    void doShowMessage(const QString &message);
    void doChatCreatedReaction(bool isCreated);
    void doDisconnected();

    void serverIpChanged();
    void serverPortChanged();

   private:
    QString m_serverIp{"127.0.0.1"}; // default values
    QString m_serverPort = "8000";
    bool m_availible = true;
    std::unique_ptr<Backend> m_backend;
  };
} // namespace elegram

#endif // BACKEND_FACADE_H
