#ifndef MESSAGEPARSER_H
#define MESSAGEPARSER_H
#include "../protobuf/messages.pb.h"
#include <memory>
#include <vector>

namespace elegram {
  class MessageParser {
   public:
    std::unique_ptr<Response> parse_response(const std::vector<uint8_t> &resp);
  };
} // namespace elegram

#endif // MESSAGEPARSER_H
