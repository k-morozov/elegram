#include "db_storage.h"

namespace elegram::server {
  bool DBStorage::login(const std::string &name, const std::string &password) {
      // todo impl
      (void) name;
      (void) password;
      return true;
  }

  bool DBStorage::registration(const std::string &name,
                               const std::string &email,
                               const std::string &password) {
      // todo impl
      (void) name;
      (void) password;
      (void) email;
      return true;
  }

  bool DBStorage::send_message(const MessageToSend &mesg) {
      // todo impl
      (void) mesg;
      return true;
  }

  std::unique_ptr<ChatsResponse> DBStorage::get_chats(uint64_t user_id) {
      // todo impl
      (void) user_id;
      return std::make_unique<ChatsResponse>();
  }

  std::unique_ptr<ContactsResponse> DBStorage::get_contacts(uint64_t user_id) {
      // todo impl
      (void) user_id;
      return std::make_unique<ContactsResponse>();
  }

  std::unique_ptr<MessagesResponse> DBStorage::get_messages(uint64_t chat_id) {
      // todo impl
      (void) chat_id;
      return std::make_unique<MessagesResponse>();
  }
} // namespace elegram::server
