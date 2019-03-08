#include "lazy_connection_wrapper.h"

namespace elegram {
  namespace server {
    LazyConnectionImpl::LazyConnectionImpl(const std::string &user_name, const std::string &dbname)
        : conn_("dbname=" + dbname + " user=" + user_name) {}

    pqxx::lazyconnection &LazyConnectionImpl::conn() {
        return conn_;
    }
  }
}