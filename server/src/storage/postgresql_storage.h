#pragma once

#include "abstract_storage.h"
#include "pimpl/lazy_connection_decl.h"

namespace elegram {
  namespace server {
    /**
     * Connection to PostgreSQL server.
     */
    class PostgresStorageConnection : public AbstractStorageConnection {
     public:
      explicit PostgresStorageConnection(const std::string &user_name, const std::string &dbname);

      bool registration(const std::string &name,
                        const std::string &email,
                        const std::string &password) override;
      /**
       * Return user's id.
       * Throw std::invalid_argument if can't login.
       * @return user's id and user's name
       */
      std::pair<uint64_t, std::string> login(const std::string &email, const std::string &password) override;
      bool send_message(uint64_t sender_id, const MessageToSend &mesg) override;
      bool add_contact(uint64_t user_id, const std::string &email) override;
      bool create_chat(uint64_t user_id, const std::string &user_name, uint64_t friend_id) override;

      std::unique_ptr<ChatsResponse> get_chats(uint64_t user_id) override;
      std::unique_ptr<ContactsResponse> get_contacts(uint64_t user_id) override;
      std::unique_ptr<MessagesResponse> get_messages(uint64_t chat_id) override;

     private:
      std::unique_ptr<LazyConnectionImpl> conn_; // Use pimpl here cause compile pqxx headers with
      // cpp-14, cause moder compiler give me warnings and errors
    };

    class PostgresStorageFactory : public AbstractStorageConnectionFactory {
     public:
      explicit PostgresStorageFactory(std::string &&user_name = "avallon", std::string &&dbname = "postgres");
      std::unique_ptr<AbstractStorageConnection> create_connection() override;
     private:
      std::string user_name_;
      std::string db_name_;
    };
  }
} // namespace elegram::server
