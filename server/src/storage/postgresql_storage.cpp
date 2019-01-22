#include "postgresql_storage.h"
#include "../hashing/hashing.h"

#include "pimpl/lazy_connection_wrapper.h"

#include <pqxx/pqxx>
#include <iostream>
#include <boost/log/trivial.hpp>

namespace elegram {
  namespace server {
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
                              "VALUES ($1, $2, $3)"
        );

        conn_->conn().prepare("get_contacts",
                              "WITH Contacts(id) AS ("
                              "  SELECT friend_id FROM ClientToContact WHERE client_id = $1"
                              ")"
                              "SELECT Client.name, Client.email"
                              "FROM Client, Contacts"
                              "WHERE Client.id = Contacts.id;"
        );

        conn_->conn().prepare("get_messages",
                              "SELECT sender_id, chat_id, text FROM Message WHERE id = $1"
        );

        conn_->conn().prepare("get_chats",
                              "WITH MyChats(chat_id) AS ("
                              "    SELECT chat_id from ClientToChat WHERE client_id = $1"
                              ") SELECT Chat.id, Chat.title FROM Chat, MyChats WHERE Chat.id = MyChats.chat_id;"
        );
    }

    bool PostgresStorageConnection::registration(const std::string &name,
                                                 const std::string &email,
                                                 const std::string &password) {
        try {
            pqxx::work txn(conn_->conn());
            pqxx::result r = txn.prepared("registration")
                (txn.quote(name))(email)
                (hash_password(password)).exec();
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

    uint64_t PostgresStorageConnection::login(const std::string &name,
                                              const std::string &password) {
        try {
            pqxx::work txn(conn_->conn());
            pqxx::result r = txn.exec("SELECT id, password_hash FROM Client WHERE name = " +
                txn.quote(name));

            if (r.empty()) {
                throw std::invalid_argument("invalid name " + name);
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

            pqxx::result r = txn.prepared("send_message")
                (sender_id)
                (mesg.chat_id())
                (txn.quote(mesg.text())).exec();

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
        std::unique_ptr<elegram::ChatsResponse> resp = std::make_unique<ChatsResponse>();
        try {
            pqxx::work txn(conn_->conn());
            pqxx::result r = txn.prepared("get_chats")(user_id).exec();
            txn.commit();

            for (auto row : r) {
                Chat *new_chat = resp->add_chats();;
                new_chat->set_chat_id(row["id"].as<uint64_t>());
                new_chat->set_title(row["title"].as<std::string>());
            }
        } catch (const pqxx::sql_error &e) {
            std::cerr << "Database error: " << e.what() << std::endl
                      << "Query was: " << e.query() << std::endl;
        } catch (const std::exception &e) {
            BOOST_LOG_TRIVIAL(error) << e.what();
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

                new_contact->set_chat_id(row["chat_id"].as<uint64_t>());
                new_contact->set_sender_id(row["sender_id"].as<uint64_t>());
                new_contact->set_text(row["mesg"].as<std::string>());
            }
        } catch (const pqxx::sql_error &e) {
            std::cerr << "Database error: " << e.what() << std::endl
                      << "Query was: " << e.query() << std::endl;
        } catch (const std::exception &e) {
            BOOST_LOG_TRIVIAL(error) << e.what();
        }
        return resp;
    }

    /* -------------------------PostgresStorageFactory-------------------------*/
    std::unique_ptr<AbstractStorageConnection> PostgresStorageFactory::create_connection() {
        return std::make_unique<PostgresStorageConnection>();
    }
  }
} // namespace elegram::server
