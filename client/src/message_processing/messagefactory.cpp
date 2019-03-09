#include "messagefactory.h"

namespace elegram {
  std::unique_ptr<Request> MessageFactory::createLoginRequest(const QString &email,
                                                              const QString &password) {
      std::unique_ptr<LoginRequest> loginRequest = std::make_unique<LoginRequest>();
      loginRequest->set_email(email.toStdString());
      loginRequest->set_password(password.toStdString());

      std::unique_ptr<Request> request = std::make_unique<Request>();
      request->set_allocated_login_request(loginRequest.release());
      return request;
  }

  std::unique_ptr<Request> MessageFactory::createRegisterRequest(const QString &name,
                                                                 const QString &email,
                                                                 const QString &password) {
      std::unique_ptr<RegisterRequest> registerRequest = std::make_unique<RegisterRequest>();
      registerRequest->set_name(name.toStdString());
      registerRequest->set_email(email.toStdString());
      registerRequest->set_password(password.toStdString());

      std::unique_ptr<Request> request = std::make_unique<Request>();
      request->set_allocated_register_request(registerRequest.release());
      return request;
  }

  std::unique_ptr<Request> MessageFactory::createAllMyChatsRequest() {
      std::unique_ptr<AllMyChatsRequest> chatsRequest = std::make_unique<AllMyChatsRequest>();
      std::unique_ptr<Request> request = std::make_unique<Request>();
      request->set_allocated_chats_request(chatsRequest.release());
      return request;
  }

  std::unique_ptr<Request> MessageFactory::createAllMyContactsRequest() {
      std::unique_ptr<AllMyContactsRequest> contactsRequest = std::make_unique<AllMyContactsRequest>();
      std::unique_ptr<Request> request = std::make_unique<Request>();
      request->set_allocated_contacts_request(contactsRequest.release());
      return request;
  }

  std::unique_ptr<Request> MessageFactory::createLoadMessagesRequest(uint64_t chatId) {
      std::unique_ptr<MessagesRequest> messagesRequest = std::make_unique<MessagesRequest>();
      messagesRequest->set_chat_id(chatId);

      std::unique_ptr<Request> request = std::make_unique<Request>();
      request->set_allocated_messages_request(messagesRequest.release());
      return request;
  }

  std::unique_ptr<Request> MessageFactory::createSendMessageRequest(uint64_t chatId,
                                                                    const QString &text) {
      std::unique_ptr<MessageToSend> mesg = std::make_unique<MessageToSend>();
      mesg->set_chat_id(chatId);
      mesg->set_text(text.toStdString());

      std::unique_ptr<SendMessageRequest> sendMessageRequest = std::make_unique<SendMessageRequest>();
      sendMessageRequest->set_allocated_mesg(mesg.release());

      std::unique_ptr<Request> request = std::make_unique<Request>();
      request->set_allocated_send_mesg_request(sendMessageRequest.release());
      return request;
  }

  std::unique_ptr<Request> MessageFactory::createAddContactRequest(const QString &email) {
      std::unique_ptr<AddContactRequest> addContactRequest = std::make_unique<AddContactRequest>();
      addContactRequest->set_email(email.toStdString());

      std::unique_ptr<Request> request = std::make_unique<Request>();
      request->set_allocated_add_contact_request(addContactRequest.release());
      return request;
  }

  std::unique_ptr<Request> MessageFactory::createCreateChatRequest(quint64 friendId) {
      std::unique_ptr<CreateChatRequest> createChatRequest = std::make_unique<CreateChatRequest>();
      createChatRequest->set_friend_id(friendId);

      std::unique_ptr<Request> request = std::make_unique<Request>();
      request->set_allocated_create_chat_request(createChatRequest.release());
      return request;
  }

} // namespace elegram
