#include "jobsutils.h"

namespace elegram {

  void setChats(Backend *backend, const ChatsResponse &chats) {
      auto vec = std::make_unique<QVector<ChatItem>>();

      for (int j = 0; j < chats.chats_size(); j++) {
          const elegram::Chat &chat = chats.chats(j);
          vec->push_back(ChatItem{chat.chat_id(), QString::fromUtf8(chat.title().c_str())});
      }
      backend->chatsList()->setItems(vec.release());
      qDebug() << "Chats setted";
  }

  void setContacts(Backend *backend, const ContactsResponse &contacts) {
      auto vec = std::make_unique<QVector<ContactItem>>();

      for (int j = 0; j < contacts.contacts_size(); j++) {
          const elegram::Contact &contact = contacts.contacts(j);
          vec->push_back(ContactItem{contact.user_id(),
                                     QString::fromUtf8(contact.name().c_str()),
                                     QString::fromUtf8(contact.email().c_str())});
      }

      backend->contactsList()->setItems(vec.release());

      qDebug() << "Contacts setted";
  }
} // namespace elegram
