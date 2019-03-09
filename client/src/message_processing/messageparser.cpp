#include "messageparser.h"
#include <cassert>
#include <QtGlobal>

namespace elegram {
  std::unique_ptr<Response> MessageParser::parse_response(const std::vector<uint8_t> &buf) {
      WrappedMessage message;
      bool readed = message.ParseFromArray(buf.data(), static_cast<int>(buf.size()));
      if (readed) {
          if (message.has_response()) {
              return std::unique_ptr<Response>{message.release_response()};
          } else { // message.has_response()
              qWarning(" got response message instead fo request");
              return nullptr;
          }
      } else {
          qWarning(" parse error");
          return nullptr;
      }
  }
} // namespace elegram
