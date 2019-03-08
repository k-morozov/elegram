#pragma once

#include <string>
#include <pqxx/binarystring>

namespace elegram {
  namespace server {
    /**
     * Function for hash passwords, cause passwords in db stores in the form of hash.
     */
    pqxx::binarystring hash_password(const std::string &password);
  } // namespace server
} // namespace elegram
