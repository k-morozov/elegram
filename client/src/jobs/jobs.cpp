#include "jobs.h"
#include "jobsutils.h"

#include "src/message_processing/messagefactory.h"
#include "src/network/network_session.h"
#include "src/backend.h"

#include <QObject>
#include <QDebug>

namespace elegram {
  LoginJob::LoginJob(Backend *backend)
      : m_backend(backend) {}

  void LoginJob::exec(const QString &email, const QString &password) {
      std::unique_ptr<elegram::Request>
          loginRequest = MessageFactory::createLoginRequest(email, password);
      auto callback = [shared_this = shared_from_this()](std::unique_ptr<Response> &&response) {
          shared_this->responseProcessingCallback(std::move(response));
      };

      m_backend->connection().request(std::move(loginRequest),
                                      std::move(callback));
  }

  void LoginJob::responseProcessingCallback(std::unique_ptr<Response> &&response) {
      if (response->has_login_response()) {
          const LoginResponse loginResponse = response->login_response();
          bool loggedIn = loginResponse.has_profile();

          if (loggedIn) {
              const Contact &profile = loginResponse.profile();
              m_backend->setProfile(std::make_unique<Profile>(profile.user_id(),
                                                              QString::fromStdString(profile.name()),
                                                              QString::fromStdString(profile.email())));

              qDebug() << "[LoginJob::responseProcessingCallback] Profile received, userName = "
                       << profile.name().c_str();

          }
          qDebug() << "[LoginJob::responseProcessingCallback] LoginJob executed with success = " << loggedIn;
          emit m_backend->loggedIn(loggedIn);

      } else {
          qWarning("[LoginJob::responseProcessingCallback] Undefined response type");
          emit m_backend->loggedIn(false);
      }
  }

  /*-------------------------------------------------------------------------------------------------*/
  RegisterJob::RegisterJob(Backend *backend)
      : m_backend(backend) {}

  void RegisterJob::exec(const QString &name, const QString &email, const QString &password) {
      std::unique_ptr<elegram::Request>
          registerRequest = MessageFactory::createRegisterRequest(name, email, password);
      auto callback = [shared_this = shared_from_this()](std::unique_ptr<Response> &&response) {
          shared_this->responseProcessingCallback(std::move(response));
      };

      m_backend->connection().request(std::move(registerRequest), std::move(callback));
  }

  void RegisterJob::responseProcessingCallback(std::unique_ptr<Response> &&response) {
      bool success = response->has_status_response() && response->status_response().result()
          == elegram::StatusResponse_RESULT::StatusResponse_RESULT_ACCEPTED;
      emit m_backend->registered(success);
      qDebug() << "[RegisterJob::responseProcessingCallback] RegisterJob executed with success = " << success;
  }

  /*-------------------------------------------------------------------------------------------------*/
  LoadChatsJob::LoadChatsJob(Backend *backend)
      : m_backend(backend) {}

  void LoadChatsJob::exec() {
      std::unique_ptr<elegram::Request> chatsRequest = MessageFactory::createAllMyChatsRequest();
      auto callback = [shared_this = shared_from_this()](std::unique_ptr<Response> &&response) {
          shared_this->responseProcessingCallback(std::move(response));
      };

      m_backend->connection().request(std::move(chatsRequest), std::move(callback));
  }

  void LoadChatsJob::responseProcessingCallback(std::unique_ptr<Response> &&response) {
      if (response->has_chats_response()) {
          setChats(m_backend, response->chats_response());
      } else {
          qWarning("Response has no chats");
      }
      emit m_backend->loadedChats();
      qDebug() << "[LoadChatsJob::responseProcessingCallback] LoadChatsJob executed ";
  }

  /*-------------------------------------------------------------------------------------------------*/
  LoadContactsJob::LoadContactsJob(Backend *backend)
      : m_backend(backend) {}

  void LoadContactsJob::exec() {
      std::unique_ptr<elegram::Request>
          contactsRequest = MessageFactory::createAllMyContactsRequest();
      auto callback = [shared_this = shared_from_this()](std::unique_ptr<Response> &&response) {
          shared_this->responseProcessingCallback(std::move(response));
      };

      m_backend->connection().request(std::move(contactsRequest), std::move(callback));
  }

  void LoadContactsJob::responseProcessingCallback(std::unique_ptr<Response> &&response) {
      if (response->has_contacts_response()) {
          setContacts(m_backend, response->contacts_response());
          qDebug() << "[LoadContactsJob::responseProcessingCallback] Contacts setted";
      } else {
          qWarning("[LoadContactsJob::responseProcessingCallback] Response has no contacts");
      }
      emit m_backend->loadedContacts();

      qDebug() << "[LoadContactsJob::responseProcessingCallback] LoadContactsJob executed ";
  }

  /*-------------------------------------------------------------------------------------------------*/
  LoadMessagesJob::LoadMessagesJob(Backend *backend)
      : m_backend(backend) {}

  void LoadMessagesJob::exec(quint64 chatId) {
      std::unique_ptr<elegram::Request>
          loadMessagesRequest = MessageFactory::createLoadMessagesRequest(chatId);
      auto callback = [shared_this = shared_from_this()](std::unique_ptr<Response> &&response) {
          shared_this->responseProcessingCallback(std::move(response));
      };

      m_backend->connection().request(std::move(loadMessagesRequest), std::move(callback));
  }

  void LoadMessagesJob::responseProcessingCallback(std::unique_ptr<Response> &&response) {
      if (response->has_messages_response()) {
          const MessagesResponse &messages = response->messages_response();
          auto vec = std::make_unique<QVector<MessageItem>>();

          for (int j = 0; j < messages.messages_size(); j++) {
              const elegram::MessageToRecieve &mesg = messages.messages(j);
              vec->push_back(MessageItem{mesg.sender_id(), QString::fromUtf8(mesg.text().c_str())});
          }

          m_backend->mesagesList()->setItems(vec.release());
      } else {
          qWarning("Response has no messages");
      }

      emit m_backend->loadedMessages();
      qDebug() << "[LoadMessagesJob::responseProcessingCallback] LoadMessagesJob executed ";
  }

  /*-------------------------------------------------------------------------------------------------*/
  SendMessageJob::SendMessageJob(Backend *backend)
      : m_backend(backend) {}

  void SendMessageJob::exec(quint64 chatId, const QString &message) {
      std::unique_ptr<elegram::Request>
          sendMessageRequest = MessageFactory::createSendMessageRequest(chatId, message);
      auto callback = [shared_this = shared_from_this()](std::unique_ptr<Response> &&response) {
          shared_this->responseProcessingCallback(std::move(response));
      };

      m_backend->connection().request(std::move(sendMessageRequest), std::move(callback));
  }

  void SendMessageJob::responseProcessingCallback(std::unique_ptr<Response> &&response) {
      bool success = response->has_status_response() && response->status_response().result()
          == elegram::StatusResponse_RESULT::StatusResponse_RESULT_ACCEPTED;
      emit m_backend->sendedMessage(success);
      qDebug() << "[SendMessageJob::responseProcessingCallback] SendMessageJob executed ";
  }

  /*-------------------------------------------------------------------------------------------------*/
  AddContactJob::AddContactJob(Backend *backend)
      : m_backend(backend) {}

  void AddContactJob::exec(const QString &email) {
      std::unique_ptr<elegram::Request>
          addContactRequest = MessageFactory::createAddContactRequest(email);
      auto callback = [shared_this = shared_from_this()](std::unique_ptr<Response> &&response) {
          shared_this->responseProcessingCallback(std::move(response));
      };

      m_backend->connection().request(std::move(addContactRequest), std::move(callback));
  }

  void AddContactJob::responseProcessingCallback(std::unique_ptr<Response> &&response) {
      bool success = response->has_status_response() && response->status_response().result()
          == elegram::StatusResponse_RESULT::StatusResponse_RESULT_ACCEPTED;
      emit m_backend->addedContact(success);
      qDebug() << "[AddContactJob::responseProcessingCallback] AddContactJob executed with success = " << success;
  }

  /*-------------------------------------------------------------------------------------------------*/
  LoadDataJob::LoadDataJob(Backend *backend)
      : m_backend(backend) {}

  void LoadDataJob::exec() {
      // firstly load contacts
      std::unique_ptr<elegram::Request>
          sendMessageRequest = MessageFactory::createAllMyContactsRequest();
      auto callback = [shared_this = shared_from_this()](std::unique_ptr<Response> &&response) {
          shared_this->responseProcessingCallback(std::move(response));
      };

      m_backend->connection().request(std::move(sendMessageRequest), std::move(callback));
  }

  void LoadDataJob::responseProcessingCallback(std::unique_ptr<Response> &&response) {
      if (response->has_contacts_response()) {
          // set contacts
          setContacts(m_backend, response->contacts_response());

          // and then load chats
          std::unique_ptr<elegram::Request>
              sendMessageRequest = MessageFactory::createAllMyChatsRequest();
          auto callback = [shared_this = shared_from_this()](std::unique_ptr<Response> &&response) {
              shared_this->responseProcessingCallback(std::move(response));
          };

          m_backend->connection().request(std::move(sendMessageRequest), std::move(callback));
      } else if (response->has_chats_response()) {
          setChats(m_backend, response->chats_response());

          emit m_backend->loadedData();
          qDebug() << "[LoadDataJob::responseProcessingCallback] LoadDataJob executed ";
      } else {
          qWarning("[LoadDataJob::responseProcessingCallback] Response has no chats or contacts");
          emit m_backend->loadedData();
      }
  }

  /*-------------------------------------------------------------------------------------------------*/
  CreateChatJob::CreateChatJob(Backend *backend)
      : m_backend(backend) {}

  void CreateChatJob::exec(quint64 friendId) {
      // firstly load contacts
      std::unique_ptr<elegram::Request>
          createChatRequest = MessageFactory::createCreateChatRequest(friendId);
      auto callback = [shared_this = shared_from_this()](std::unique_ptr<Response> &&response) {
          shared_this->responseProcessingCallback(std::move(response));
      };

      m_backend->connection().request(std::move(createChatRequest), std::move(callback));
  }

  void CreateChatJob::responseProcessingCallback(std::unique_ptr<Response> &&response) {
      bool success = response->has_status_response() && response->status_response().result()
          == elegram::StatusResponse_RESULT::StatusResponse_RESULT_ACCEPTED;
      emit m_backend->chatCreated(success);
      qDebug() << "[CreateChatJob::responseProcessingCallback] CreateChatJob executed ";
  }

} // namespace elegram
