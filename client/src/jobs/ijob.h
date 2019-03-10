#ifndef IJOBS_H
#define IJOBS_H

#include "../protobuf/messages.pb.h"
#include <memory>

namespace elegram {
  class Backend; // forward decl

  class IJob : public std::enable_shared_from_this<IJob> {
   public:
    /**
     * Callback called from session::onRead after reading message
     */
    virtual void responseProcessingCallback(std::unique_ptr<Response> &&response) = 0;
    virtual ~IJob() = default;
  };
} // namespace elegram

#endif // IJOBS_H
