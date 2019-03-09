#include "backend.h"

namespace elegram {
  Profile::Profile(quint64 userId, QString &&userName, QString &&userEmail)
      : m_userId(userId), m_userName(std::move(userName)), m_userEmail(std::move(userEmail)) {}

  quint64 Profile::userId() const {
      return m_userId;
  }

  QString Profile::userName() const {
      return m_userName;
  }

  QString Profile::userEmail() const {
      return m_userEmail;
  }

  /*---------------------------------------------------------------------------------------------*/
  Backend::Backend(ChatsList *chatsList, ContactsList *contactsList, MessagesList *mesgList,
                   const QString &serverIp, const QString &serverPort,
                   QObject *parent)
      : QObject(parent), m_chatsList(chatsList), m_contactsList(contactsList), m_mesagesList(mesgList) {
      m_session = std::make_unique<Session>(this);
      m_session->connect(serverIp, serverPort);
  }

  ChatsList *Backend::chatsList() const {
      return m_chatsList;
  }

  ContactsList *Backend::contactsList() const {
      return m_contactsList;
  }

  MessagesList *Backend::mesagesList() const {
      return m_mesagesList;
  }

  Session &Backend::connection() const {
      return *m_session;
  }

  quint64 Backend::userId() const {
      if (m_profile) {
          return m_profile->userId();
      } else {
          qWarning("Profile not setted, but requested!");
          return 0;
      }
  }

  QString Backend::userName() const {
      if (m_profile) {
          return m_profile->userName();
      } else {
          qWarning("Profile not setted, but requested!");
          return "Undefined";
      }
  }

  QString Backend::userEmail() const {
      if (m_profile) {
          return m_profile->userEmail();
      } else {
          qWarning("Profile not setted, but requested!");
          return "Undefined";
      }
  }

  void Backend::setProfile(std::unique_ptr<Profile> &&profile) {
      m_profile = std::move(profile);
  }

  void Backend::signin(const QString &email, const QString &password) {
      std::make_shared<LoginJob>(this)->exec(email, password);
  }

  void Backend::signup(const QString &name, const QString &email, const QString &password) {
      std::make_shared<RegisterJob>(this)->exec(name, email, password);
  }

  void Backend::sendMessage(quint64 chatId, const QString &message) {
      std::make_shared<SendMessageJob>(this)->exec(chatId, message);
  }

  void Backend::addContact(const QString &email) {
      std::make_shared<AddContactJob>(this)->exec(email);
  }

  void Backend::loadChats() {
      std::make_shared<LoadChatsJob>(this)->exec();
  }

  void Backend::loadContacts() {
      std::make_shared<LoadContactsJob>(this)->exec();
  }

  void Backend::loadData() {
      std::make_shared<LoadDataJob>(this)->exec();
  }

  void Backend::loadMessages(quint64 chatId) {
      std::make_shared<LoadMessagesJob>(this)->exec(chatId);
  }

  bool Backend::updateConnectionSettings(const QString &ip, const QString &port) {
      QVariant res = m_session->connect(ip, port);

      if (res.canConvert<QString>()) {
          // res contain error description
          emit updatedConnectionSettings(res.value<QString>());
          return false;
      } else {
          // it's okay, no error, parameters are valid
          emit updatedConnectionSettings("");
          return true;
      }
  }

  void Backend::createChat(quint64 friendId) {
      std::make_shared<CreateChatJob>(this)->exec(friendId);
  }

} // namespace elegram
