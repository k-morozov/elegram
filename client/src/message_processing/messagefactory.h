#ifndef MESSAGEFACTORY_H
#define MESSAGEFACTORY_H
#include <QString>
#include <memory>
#include "../protobuf/messages.pb.h"

namespace elegram {
  class MessageFactory {
   public:
    static std::unique_ptr<Request> createLoginRequest(const QString &name, const QString &password);
    static std::unique_ptr<Request> createRegisterRequest(const QString &name, const QString &email,
                                                          const QString &password);
    static std::unique_ptr<Request> createAllMyChatsRequest();
    static std::unique_ptr<Request> createAllMyContactsRequest();
    static std::unique_ptr<Request> createLoadMessagesRequest(uint64_t chatId);
    static std::unique_ptr<Request> createSendMessageRequest(uint64_t chatId, const QString &text);
    static std::unique_ptr<Request> createAddContactRequest(const QString &email);
    static std::unique_ptr<Request> createCreateChatRequest(quint64 friendId);
  };
} // namespace elegram

#endif // MESSAGEFACTORY_H
