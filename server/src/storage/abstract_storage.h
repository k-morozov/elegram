#pragma once

#include <string>
#include "../proto/messages.pb.h"

namespace elegram {
  namespace server {
    class AbstractStorageConnection {
     public:
      virtual bool registration(const std::string &name,
                                const std::string &email,
                                const std::string &password) = 0;
      /**
       * Return user's id.
       * Throw std::invalid_argument if can't login.
       * @return user's id and user's name
       */
      virtual std::pair<uint64_t, std::string> login(const std::string &email, const std::string &password) = 0;
      virtual bool send_message(uint64_t sender_id, const MessageToSend &mesg) = 0;
      virtual bool add_contact(uint64_t user_id, const std::string &email) = 0;
      virtual bool create_chat(uint64_t user_id, std::shared_ptr<std::string> &&user_name, uint64_t friend_id) = 0;

      virtual std::unique_ptr<ChatsResponse> get_chats(uint64_t user_id) = 0;
      virtual std::unique_ptr<ContactsResponse> get_contacts(uint64_t user_id) = 0;
      virtual std::unique_ptr<MessagesResponse> get_messages(uint64_t chat_id) = 0;

      virtual ~AbstractStorageConnection() = default;
    };

    class AbstractStorageConnectionFactory {
     public:
      virtual std::unique_ptr<AbstractStorageConnection> create_connection() = 0;
      virtual ~AbstractStorageConnectionFactory() = default;
    };
  } // namespace server
} // namespace elegram
