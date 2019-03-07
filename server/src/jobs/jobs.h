#pragma once

#include "../server/session.h"
#include "../proto/messages.pb.h"

#include <string>
#include <memory>

namespace elegram::server {
  /**
   * Todo write all jobs that sessions generate here
   */
  class ParseCommandJob {
   public:
    explicit ParseCommandJob(std::shared_ptr<ClientSession> session);
    void operator()();

   private:
    std::shared_ptr<ClientSession> session_;
  };

  /**
   * Protobuf Requests
   * ----------------------------------------------------------------------------------------------------
   */
  class RegisterRequestJob {
   public:
    RegisterRequestJob(RegisterRequest *mesg, std::shared_ptr<ClientSession> session);
    void operator()();

   private:
    std::shared_ptr<ClientSession> session_;
    std::unique_ptr<RegisterRequest> mesg_;
  };

  class LoginRequestJob {
   public:
    LoginRequestJob(LoginRequest *mesg, std::shared_ptr<ClientSession> session);
    void operator()();

   private:
    std::shared_ptr<ClientSession> session_;
    std::unique_ptr<LoginRequest> mesg_;
  };

  class ChatsRequestJob {
   public:
    explicit ChatsRequestJob(std::shared_ptr<ClientSession> session);
    void operator()();

   private:
    std::shared_ptr<ClientSession> session_;
  };

  class ContactsRequestJob {
   public:
    explicit ContactsRequestJob(std::shared_ptr<ClientSession> session);
    void operator()();

   private:
    std::shared_ptr<ClientSession> session_;
  };

  class MessagesRequestJob {
   public:
    MessagesRequestJob(MessagesRequest *mesg, std::shared_ptr<ClientSession> session);
    void operator()();

   private:
    std::shared_ptr<ClientSession> session_;
    std::unique_ptr<MessagesRequest> mesg_;
  };

  class SendMesgRequestJob {
   public:
    SendMesgRequestJob(SendMessageRequest *mesg, std::shared_ptr<ClientSession> session);
    void operator()();

   private:
    std::shared_ptr<ClientSession> session_;
    std::unique_ptr<SendMessageRequest> mesg_;
  };

  class AddContactRequestJob {
   public:
    AddContactRequestJob(AddContactRequest *mesg, std::shared_ptr<ClientSession> session);
    void operator()();

   private:
    std::shared_ptr<ClientSession> session_;
    std::unique_ptr<AddContactRequest> mesg_;
  };

  class CreateChatRequestJob {
   public:
    CreateChatRequestJob(CreateChatRequest *mesg, std::shared_ptr<ClientSession> session);
    void operator()();

   private:
    std::shared_ptr<ClientSession> session_;
    std::unique_ptr<CreateChatRequest> mesg_;
  };

} // namespace elegram::server
