#pragma once

#include <stdexcept>

namespace elegram::exception {
  class terminate_exception : public std::runtime_error {
   public:
    explicit terminate_exception(const char *what_arg);
  };
} // namespace elegram::exception
