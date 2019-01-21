#pragma once

#include <boost/asio.hpp>
#include <boost/asio/thread_pool.hpp>

#include "../jobs/jobs.h"
#include "../storage/abstract_storage.h"

#include "session.h"

namespace elegram::server {
  namespace ba = boost::asio;

  class ElegramServer : public std::enable_shared_from_this<ElegramServer> {
   public:
    explicit ElegramServer(unsigned short port,
                           std::shared_ptr<AbstractStorageConnectionFactory> &&storage_ptr);
    ~ElegramServer();
    void run();

   private:
    void handle_accept(const std::shared_ptr<ClientSession> &connected_client,
                       const boost::system::error_code &err);
    void signal_handler(const boost::system::error_code &, int);

   private:
    ba::io_service network_service_{};
    ba::ip::tcp::endpoint ep_;
    ba::ip::tcp::acceptor acceptor_;

    std::shared_ptr<ba::thread_pool> job_pool_ = std::make_shared<ba::thread_pool>(2);
    std::shared_ptr<AbstractStorageConnectionFactory> db_stor_;
  };

} // namespace elegram::server
