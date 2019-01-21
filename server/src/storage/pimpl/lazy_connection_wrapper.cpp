#include "lazy_connection_wrapper.h"

namespace elegram {
  namespace server {
    pqxx::lazyconnection &LazyConnectionImpl::conn() {
        return conn_;
    }
  }
}