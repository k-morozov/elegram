#pragma once

#include "lazy_connection_decl.h"
#include <pqxx/connection>

namespace elegram {
  namespace server {
    class LazyConnectionImpl {
     public:
      LazyConnectionImpl(const std::string &user_name, const std::string &dbname);
      pqxx::lazyconnection &conn();
     private:
      pqxx::lazyconnection conn_;
    };
  } // namespace server
} // namespace elegram

