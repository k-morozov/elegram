#include <boost/log/trivial.hpp>

#include "server.h"
#include "session.h"

namespace elegram::server {
  ElegramServer::ElegramServer(unsigned short port,
                               std::shared_ptr<AbstractStorageConnectionFactory> &&storage_ptr)
      : ep_(ba::ip::tcp::v4(), port),
        acceptor_(network_service_, ep_),
        db_stor_(std::move(storage_ptr)) {}

  void ElegramServer::run() {
      using namespace std::placeholders;

      ba::signal_set sig(network_service_, SIGINT, SIGABRT);
      sig.async_wait(std::bind(&ElegramServer::signal_handler,
                               shared_from_this(),
                               std::placeholders::_1,
                               std::placeholders::_2));

      auto clientSession = std::make_shared<ClientSession>(network_service_, job_pool_,
                                                           db_stor_->create_connection());
      acceptor_.async_accept(clientSession->sock(),
                             std::bind(&ElegramServer::handle_accept, shared_from_this(),
                                       clientSession, _1));

      network_service_.run();
  }

  void ElegramServer::handle_accept(const std::shared_ptr<ClientSession> &connected_client,
                                    const boost::system::error_code &err) {
      if (err == boost::system::errc::operation_canceled) {
          return;
      } else if (err) {
          throw std::runtime_error(std::string{"Unhandled error: "} + err.message());
      }

      using namespace std::placeholders;
      BOOST_LOG_TRIVIAL(info) << connected_client << " connected";

      connected_client->start();

      // async wait new connection
      auto waited_session =
          std::make_shared<ClientSession>(network_service_, job_pool_, db_stor_->create_connection());
      acceptor_.async_accept(waited_session->sock(),
                             std::bind(&ElegramServer::handle_accept,
                                       shared_from_this(),
                                       waited_session, _1));
  }

  ElegramServer::~ElegramServer() {
      BOOST_LOG_TRIVIAL(info) << "### Server DTOR called";
      // todo pimpl
  }

  void ElegramServer::signal_handler(const boost::system::error_code &, int) {
      BOOST_LOG_TRIVIAL(info) << "Signal handler called";
      acceptor_.close();

      // requests_queue_->stop_wait_and_block_pushing();
      // todo stop doing this
  }

} // namespace elegram::server
