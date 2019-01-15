#pragma once

#include "../proto/messages.pb.h"
#include <string>

namespace elegram::server {
  class DBStorage {
   public:
    bool registration(const std::string &name, const std::string &email, const std::string &password);
    bool login(const std::string &name, const std::string &password);
    bool send_message(const MessageToSend &mesg);
    std::unique_ptr<ChatsResponse> get_chats(uint64_t user_id);
    std::unique_ptr<ContactsResponse> get_contacts(uint64_t user_id);
    std::unique_ptr<MessagesResponse> get_messages(uint64_t chat_id);

   private:
    // todo impl
    // todo add Caching
  };
} // namespace elegram::server
