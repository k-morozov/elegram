#include "exceptions.h"

namespace elegram::exception {
  terminate_exception::terminate_exception(const char *what_arg)
      : runtime_error(what_arg) {}
} // namespace elegram::exception
