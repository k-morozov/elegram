#pragma once

#include "abstract_storage.h"
#include "pimpl/lazy_connection_decl.h"

/**
 * If your program's database interaction is not as efficient as it needs to be, the first place to
 * look is usually the SQL you're executing. But libpqxx has a few specialized features to help you
 * squeeze a bit more performance out of how you issue commands and retrieve data:
 *
 * - Streams. Use these as a faster way to transfer data between your code and the database.
 * - Prepared statements. These can be executed many times without the server parsing and planning them
 * each and every time. They also save you having to escape string parameters.
 * - pqxx::pipeline lets you send queries to the database in batch, and continue other processing while
 * they are executing.
 *
 * As always of course, don't risk the quality of your code for optimizations that you don't need!
 */

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
      uint64_t login(const std::string &name, const std::string &password) override;
      bool send_message(uint64_t sender_id, const MessageToSend &mesg) override;
      std::unique_ptr<ChatsResponse> get_chats(uint64_t user_id) override;
      std::unique_ptr<ContactsResponse> get_contacts(uint64_t user_id) override;
      std::unique_ptr<MessagesResponse> get_messages(uint64_t chat_id) override;

     private:
      std::unique_ptr<LazyConnectionImpl> conn_;
//      pqxx::lazyconnection conn_{"dbname=postgres user=postgres"};
    };

    class PostgresStorageFactory : public AbstractStorageConnectionFactory {
     public:
      std::unique_ptr<AbstractStorageConnection> create_connection() override;
    };
  }
} // namespace elegram::server
