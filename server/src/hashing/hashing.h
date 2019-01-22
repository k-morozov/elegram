#pragma once

#include <string>
#include <pqxx/binarystring>

namespace elegram {
  namespace server {
    pqxx::binarystring hash_password(const std::string &password);
  } // namespace server
} // namespace elegram
