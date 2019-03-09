#include "backend_facade.h"
#include "jobs/jobs.h"
#include "backend.h"

#include <QDebug>

namespace elegram {
  BackEndFacade::BackEndFacade(ChatsList *chatsList, ContactsList *contactsList, MessagesList *mesgList,
                               QObject *parent) :
      QObject(parent),
      m_backend(std::make_unique<Backend>(chatsList, contactsList, mesgList, m_serverIp, m_serverPort)) {
      qDebug() << "Backend ctor called";

      connect(m_backend.get(), &Backend::loggedIn, [this](bool status) {
          setAvailible(true);
          emit doLoginReaction(status);
      });

      connect(m_backend.get(), &Backend::registered, [this](bool status) {
          setAvailible(true);
          emit doRegisterReaction(status);
      });

      connect(m_backend.get(), &Backend::loadedChats, [this]() {
          setAvailible(true);
          emit doLoadChatsReaction();
      });

      connect(m_backend.get(), &Backend::loadedContacts, [this]() {
          setAvailible(true);
          emit doLoadContactsReaction();
      });

      connect(m_backend.get(), &Backend::loadedMessages, [this]() {
          setAvailible(true);
          emit doLoadMessagesReaction();
      });

      connect(m_backend.get(), &Backend::sendedMessage, [this](bool status) {
          setAvailible(true);
          emit doSendMessageReaction(status);
      });

      connect(m_backend.get(), &Backend::addedContact, [this](bool isAdded) {
          setAvailible(true);
          emit doAddContactReaction(isAdded);
      });

      connect(m_backend.get(), &Backend::loadedData, [this]() {
          setAvailible(true);
          emit doLoadDataReaction();
      });

      connect(m_backend.get(), &Backend::updatedConnectionSettings, [this](const QString &statusMesg) {
          setAvailible(true);
          emit doUpdateConnectionSettingsReaction(statusMesg);
      });

      connect(m_backend.get(), &Backend::operationAborted, [this](const QString &errorMessage) {
          setAvailible(true);
          emit doOperationAborted(errorMessage);
      });

      connect(m_backend.get(), &Backend::showMessage, [this](const QString &message) {
          emit doShowMessage(message);
      });

      connect(m_backend.get(), &Backend::chatCreated, [this](bool isCreated) {
          setAvailible(true);
          emit doChatCreatedReaction(isCreated);
      });

      connect(m_backend.get(), &Backend::disconnected, [this]() {
          setAvailible(true);
          emit doDisconnected();
      });
  }

  bool BackEndFacade::availible() {
      return m_availible;
  }

  void BackEndFacade::setAvailible(bool availible) {
      m_availible = availible;
  }

  void BackEndFacade::setServerIp(const QString &serverIp) {
      if (m_serverIp != serverIp) {
          m_serverIp = serverIp;
          emit serverIpChanged();
      }
  }

  QString BackEndFacade::serverIp() const {
      return m_serverIp;
  }

  void BackEndFacade::setServerPort(const QString &port) {
      if (m_serverPort != port) {
          m_serverPort = port;
          emit serverPortChanged();
      }
  }

  quint64 BackEndFacade::userId() const {
      return m_backend->userId();
  }

  QString BackEndFacade::userName() const {
      return m_backend->userName();
  }

  QString BackEndFacade::userEmail() const {
      return m_backend->userEmail();
  }

  QString BackEndFacade::serverPort() const {
      return m_serverPort;
  }

  void BackEndFacade::signin(const QString &email, const QString &password) {
      if (availible()) {
          setAvailible(false);
          m_backend->signin(email, password);
      }
  }

  void BackEndFacade::signup(const QString &name, const QString &email, const QString &password) {
      if (availible()) {
          setAvailible(false);
          m_backend->signup(name, email, password);
      }
  }

  void BackEndFacade::sendMessage(quint64 chatId, const QString &message) {
      if (availible()) {
          setAvailible(false);
          m_backend->sendMessage(chatId, message);
      }
  }

  void BackEndFacade::addContact(const QString &email) {
      if (availible()) {
          setAvailible(false);
          m_backend->addContact(email);
      }
  }

  void BackEndFacade::loadChats() {
      if (availible()) {
          setAvailible(false);
          m_backend->loadChats();
      }
  }

  void BackEndFacade::loadContacts() {
      if (availible()) {
          setAvailible(false);
          m_backend->loadContacts();
      }
  }

  void BackEndFacade::loadData() {
      if (availible()) {
          setAvailible(false);
          m_backend->loadData();
      }
  }

  void BackEndFacade::loadMessages(quint64 chatId) {
      if (availible()) {
          setAvailible(false);
          m_backend->loadMessages(chatId);
      }
  }

  void BackEndFacade::updateConnectionSettings(const QString &ip, const QString &port) {
      if (availible()) {
          setAvailible(false);
          if (m_backend->updateConnectionSettings(ip, port)) {
              setServerIp(ip);
              setServerPort(port);
          }
      }
  }

  void BackEndFacade::createChat(quint64 friendId) {
      if (availible()) {
          setAvailible(false);
          m_backend->createChat(friendId);
      }
  }

  QString BackEndFacade::get_contact_name(quint64 sender_id) {
      if (sender_id == userId()) {
          return userName();
      } else {
          return m_backend->contactsList()->getUserBy(sender_id);
      }
  }
} // namespace elegram
