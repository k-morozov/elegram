#pragma once

#include "abstract_storage.h"
#include "pimpl/lazy_connection_decl.h"

namespace elegram {
  namespace server {
    class PostgresStorageConnection : public AbstractStorageConnection {
     public:
      PostgresStorageConnection();

      bool registration(const std::string &name,
                        const std::string &email,
                        const std::string &password) override;
      /**
       * Return user's id.
       * Throw std::invalid_argument if can't login.
       * @return user's id
       */
      uint64_t login(const std::string &email, const std::string &password) override;
      bool send_message(uint64_t sender_id, const MessageToSend &mesg) override;

      std::unique_ptr<ChatsResponse> get_chats(uint64_t user_id) override;
      std::unique_ptr<ContactsResponse> get_contacts(uint64_t user_id) override;
      std::unique_ptr<MessagesResponse> get_messages(uint64_t chat_id) override;

     private:
      std::unique_ptr<LazyConnectionImpl> conn_; // Use pimpl here cause compile pqxx headers with
      // cpp-14, cause moder compiler give me warnings and errors
    };

    class PostgresStorageFactory : public AbstractStorageConnectionFactory {
     public:
      std::unique_ptr<AbstractStorageConnection> create_connection() override;
    };
  }
} // namespace elegram::server
