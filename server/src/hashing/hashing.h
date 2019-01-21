#pragma once

#include <string>

namespace elegram {
  namespace server {
    std::string hash_password(const std::string &password);
  } // namespace server
} // namespace elegram
