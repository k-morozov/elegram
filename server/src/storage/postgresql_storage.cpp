#include "postgresql_storage.h"
#include "../hashing/hashing.h"

#include "pimpl/lazy_connection_wrapper.h"

#include <pqxx/pqxx>
#include <iostream>
#include <boost/log/trivial.hpp>

namespace elegram {
  namespace server {
    namespace {
      bool can_write(pqxx::work &txn, uint64_t sender_id, uint64_t chat_id) {
          try {
              pqxx::result r = txn.prepared("check_sender_able_to_write_to_chat")
                  (sender_id)(chat_id).exec();
              return !r.empty();
          } catch (const pqxx::sql_error &e) {
              std::cerr << "Database error: " << e.what() << std::endl
                        << "Query was: " << e.query() << std::endl;
              return false;
          } catch (const std::exception &e) {
              BOOST_LOG_TRIVIAL(error) << "exception in can_write(): " << e.what();
              return false;
          }
      }
    } // anonymous namespace

    /**
     * TODO don't prepare them for every connection, do it once for all connections (can we do it?)
     */
    PostgresStorageConnection::PostgresStorageConnection()
        : conn_(std::make_unique<LazyConnectionImpl>()) {
        conn_->conn().prepare("registration",
                              "INSERT INTO Client (name, email, password_hash) "
                              "VALUES ( $1, $2, $3 )"
        );

        conn_->conn().prepare("send_message",
                              "INSERT INTO Message(sender_id, chat_id, mesg) "
                              "VALUES ( $1, $2, $3 ) "
        );

        conn_->conn().prepare("add_contact",
                              "INSERT INTO ClientToContact(client_id, friend_id) "
                              "VALUES ( $1, $2 ) "
        );

        conn_->conn().prepare("get_user_by_email",
                              "SELECT id, name FROM Client WHERE email = $1 "
        );

        conn_->conn().prepare("get_contacts",
                              "WITH Contacts(id) AS ( "
                              "  SELECT friend_id FROM ClientToContact WHERE client_id = $1 "
                              ") "
                              "SELECT Client.id, Client.name, Client.email "
                              "FROM Client, Contacts "
                              "WHERE Client.id = Contacts.id "
        );

        conn_->conn().prepare("get_messages",
                              "SELECT sender_id, mesg FROM Message WHERE chat_id = $1 "
        );

        conn_->conn().prepare("get_chats",
                              "WITH MyChats(chat_id) AS ( "
                              "    SELECT chat_id from ClientToChat WHERE client_id = $1 "
                              ") SELECT Chat.id, Chat.title FROM Chat, MyChats "
                              "WHERE Chat.id = MyChats.chat_id "
        );

        conn_->conn().prepare("check_sender_able_to_write_to_chat",
                              "SELECT 1 FROM ClientToChat "
                              "WHERE client_id = $1 AND chat_id = $2 "
        );
    }

    bool PostgresStorageConnection::registration(const std::string &name,
                                                 const std::string &email,
                                                 const std::string &password) {
        try {
            pqxx::work txn(conn_->conn());
            pqxx::result r = txn.prepared("registration")
                (name)
                (email)
                (hash_password(password)).exec();
            // todo make this user friend to itself
            txn.commit();
        } catch (const pqxx::sql_error &e) {
            std::cerr
                << "Database error: " << e.what() << std::endl
                << "Query was: " << e.query() << std::endl;
            return false;
        } catch (const std::exception &e) {
            BOOST_LOG_TRIVIAL(error) << e.what();
            return false;
        }

        return true;
    }

    uint64_t PostgresStorageConnection::login(const std::string &email,
                                              const std::string &password) {
        try {
            pqxx::work txn(conn_->conn());
            pqxx::result r = txn.exec("SELECT id, password_hash FROM Client WHERE email = " +
                txn.quote(email));

            if (r.empty()) {
                throw std::invalid_argument("invalid email " + email);
            } else if (r.size() == 1
                && pqxx::binarystring{r[0]["password_hash"]} != hash_password(password)) {
                throw std::invalid_argument("Invalid password");
            } else {
                return r[0]["id"].as<uint64_t>();
            }
        } catch (const pqxx::sql_error &e) {
            std::cerr
                << "Database error: " << e.what() << std::endl
                << "Query was: " << e.query() << std::endl;
            throw std::runtime_error("Database error");
        } catch (...) {
            throw; // rethrow
        }
    }

    bool PostgresStorageConnection::send_message(uint64_t sender_id,
                                                 const elegram::MessageToSend &mesg) {
        try {
            pqxx::work txn(conn_->conn());

            if (!can_write(txn, sender_id, mesg.chat_id())) {
                throw std::invalid_argument("invalid chat_id for this sender_id");
            }

            pqxx::result r = txn.prepared("send_message")
                (sender_id)
                (mesg.chat_id())
                (mesg.text()).exec();
            txn.commit();
        } catch (const pqxx::sql_error &e) {
            std::cerr << "Database error: " << e.what() << std::endl
                      << "Query was: " << e.query() << std::endl;
            return false;
        }
        catch (const std::exception &e) {
            BOOST_LOG_TRIVIAL(error) << e.what();
            return false;
        }

        return true;
    }

    bool PostgresStorageConnection::add_contact(uint64_t user_id, const std::string &email) {
        try {
            pqxx::work txn(conn_->conn());

            pqxx::result req = txn.prepared("get_user_by_email")(email).exec();
            if (req.empty()) {
                return false;
            }

            uint64_t contact_id = req[0]["id"].as<uint64_t>();
            pqxx::result r = txn.prepared("add_contact")(user_id)(contact_id).exec();
            txn.commit();
        } catch (const pqxx::sql_error &e) {
            std::cerr << "Database error: " << e.what() << std::endl
                      << "Query was: " << e.query() << std::endl;
            return false;
        }
        catch (const std::exception &e) {
            BOOST_LOG_TRIVIAL(error) << e.what();
            return false;
        }

        return true;
    }

    // todo change signature and protocol for db error reporting
    std::unique_ptr<elegram::ChatsResponse> PostgresStorageConnection::get_chats(uint64_t user_id) {
        BOOST_LOG_TRIVIAL(error) << "executing ChatsRequest";
        std::unique_ptr<elegram::ChatsResponse> resp = std::make_unique<ChatsResponse>();
        try {
            pqxx::work txn(conn_->conn());
            pqxx::result r = txn.prepared("get_chats")(user_id).exec();
            txn.commit();

            for (auto row : r) {
                Chat *new_chat = resp->add_chats();;
                new_chat->set_chat_id(row["id"].as<uint64_t>());
                if (!row["title"].is_null()) {
                    new_chat->set_title(row["title"].as<std::string>());
                }
            }
        } catch (const pqxx::sql_error &e) {
            std::cerr << "Database error: " << e.what() << std::endl
                      << "Query was: " << e.query() << std::endl;
        } catch (const std::exception &e) {
            BOOST_LOG_TRIVIAL(error) << "Exception in get_chats(uint64_t user_id): " << e.what();
        }
        return resp;
    }

    std::unique_ptr<elegram::ContactsResponse> PostgresStorageConnection::get_contacts(uint64_t user_id) {
        std::unique_ptr<elegram::ContactsResponse> resp = std::make_unique<ContactsResponse>();
        try {
            pqxx::work txn(conn_->conn());
            pqxx::result r = txn.prepared("get_contacts")(user_id).exec();
            txn.commit();

            for (auto row : r) {
                Contact *new_contact = resp->add_contacts();;
                new_contact->set_user_id(row["id"].as<uint64_t>());
                new_contact->set_name(row["name"].as<std::string>());
                new_contact->set_email(row["email"].as<std::string>());
            }
        } catch (const pqxx::sql_error &e) {
            std::cerr << "Database error: " << e.what() << std::endl
                      << "Query was: " << e.query() << std::endl;
        } catch (const std::exception &e) {
            BOOST_LOG_TRIVIAL(error) << e.what();
        }
        return resp;
    }

    std::unique_ptr<elegram::MessagesResponse> PostgresStorageConnection::get_messages(uint64_t chat_id) {
        std::unique_ptr<elegram::MessagesResponse> resp = std::make_unique<MessagesResponse>();
        try {
            pqxx::work txn(conn_->conn());
            pqxx::result r = txn.prepared("get_messages")(chat_id).exec();
            txn.commit();

            for (auto row : r) {
                MessageToRecieve *new_contact = resp->add_messages();

                new_contact->set_sender_id(row["sender_id"].as<uint64_t>());
                new_contact->set_text(row["mesg"].as<std::string>());
            }
        } catch (const pqxx::sql_error &e) {
            std::cerr << "Database error: " << e.what() << std::endl
                      << "Query was: " << e.query() << std::endl;
        } catch (const std::exception &e) {
            BOOST_LOG_TRIVIAL(error) << "Exception in get_message: " << e.what();
        }
        return resp;
    }

    /* -------------------------PostgresStorageFactory-------------------------*/
    std::unique_ptr<AbstractStorageConnection> PostgresStorageFactory::create_connection() {
        return std::make_unique<PostgresStorageConnection>();
    }
  }
} // namespace elegram::server
