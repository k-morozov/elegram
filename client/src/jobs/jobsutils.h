#pragma once

#include "src/backend.h"

namespace elegram {
  void setChats(Backend *backend, const ChatsResponse &chats);
  void setContacts(Backend *backend, const ContactsResponse &contacts);
} // namespace elegram




