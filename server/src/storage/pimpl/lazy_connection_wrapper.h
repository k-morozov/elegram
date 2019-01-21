#pragma once

#include "lazy_connection_decl.h"
#include <pqxx/connection>

namespace elegram {
  namespace server {
    class LazyConnectionImpl {
     public:
      pqxx::lazyconnection &conn();
     private:
      pqxx::lazyconnection conn_{"dbname=postgres user=postgres"};
    };
  } // namespace server
} // namespace elegram

