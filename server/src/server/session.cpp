#include <boost/log/trivial.hpp>
#include <boost/asio/post.hpp>
#include <functional>

#include "session.h"
#include "../jobs/jobs.h"

namespace elegram::server {
  ClientState::ClientState(uint64_t user_id)
      : user_id_(user_id) {}

  uint64_t ClientState::user_id() const {
      return user_id_;
  }

  ClientSession::ClientSession(ba::io_service &service,
                               std::shared_ptr<ba::thread_pool> job_pool,
                               std::shared_ptr<DBStorage> storage) :
      service_(service), sock_(service_), job_pool_(std::move(job_pool)), storage_(std::move(storage)) {
      BOOST_LOG_TRIVIAL(info) << this << " Session CTOR called";
  }

  ClientSession::~ClientSession() {
      BOOST_LOG_TRIVIAL(info) << this << " Session DTOR called";
  }

  boost::asio::ip::tcp::socket &ClientSession::sock() {
      return sock_;
  }

  const std::shared_ptr<ba::thread_pool> &ClientSession::job_pool() {
      return job_pool_;
  }

  const std::shared_ptr<DBStorage> &ClientSession::storage() {
      return storage_;
  }

  void ClientSession::start() {
      stopped_ = false;
      do_async_read_prefix();
  }

  void ClientSession::stop() {
      if (stopped_) {
          return;
      }
      stopped_ = true;
      sock_.close();
      BOOST_LOG_TRIVIAL(info) << this << " Stopped, sock closed !";
  }

  void ClientSession::on_read_prefix(const ClientSession::error_code &err, size_t bytes) {
      if (err) {
          stop();
      }
      if (stopped_) {
          return;
      }

      if (bytes == HEADER_SIZE) {
          LengthPrefix prefix;
          bool parsed = prefix.ParseFromArray(read_buffer_.data(),
                                              static_cast<int>(read_buffer_.size()));
          if (parsed) {
              do_async_read_message(prefix.length());
          } else {
              stop();
              BOOST_LOG_TRIVIAL(error) << this << " can't parse header";
              return;
          }
      } else {
          stop();
          BOOST_LOG_TRIVIAL(error) << this << " wrong header size";
          return;
      }
  }

  void ClientSession::do_async_read_prefix() {
      if (stopped_) {
          return;
      }
      using namespace std::placeholders;

      // BOOST_LOG_TRIVIAL(info) << this << " Will async_read_until now";
      read_buffer_.resize(HEADER_SIZE);
      ba::async_read(sock_,
                     ba::buffer(read_buffer_),
                     std::bind(&ClientSession::on_read_prefix, shared_from_this(), _1, _2));
  }

  void ClientSession::on_read_message(const ClientSession::error_code &err, size_t readed_bytes,
                                      uint64_t mesg_size) {
      if (err) {
          stop();
      }
      if (stopped_) {
          return;
      }

      if (readed_bytes == mesg_size) {
          ba::post(*job_pool_, ParseCommandJob{shared_from_this()});
      } else {
          stop();
          BOOST_LOG_TRIVIAL(error) << this << " wrong message size";
          return;
      }
  }

  void ClientSession::do_async_read_message(uint64_t mesg_size) {
      if (stopped_) {
          return;
      }
      using namespace std::placeholders;

      // BOOST_LOG_TRIVIAL(info) << this << " Will async_read_until now";
      read_buffer_.resize(mesg_size);
      ba::async_read(sock_,
                     ba::buffer(read_buffer_),
                     std::bind(&ClientSession::on_read_message, shared_from_this(), _1, _2, mesg_size));
  }

  void ClientSession::write(const WrappedMessage &mesg) {
//      BOOST_LOG_TRIVIAL(info) << this << " will write " << s.size() << " bytes";
      do_async_write(mesg);
  }

  void ClientSession::on_write(const ClientSession::error_code &err,
                               [[maybe_unused]] size_t bytes) {
      if (err) {
          stop();
      }
      if (stopped_) {
          return;
      }

      do_async_read_prefix();
  }

  void ClientSession::do_async_write(const WrappedMessage &mesg) {
      if (stopped_) {
          return;
      }
      using namespace std::placeholders;

      read_buffer_.resize(static_cast<unsigned long>(mesg.ByteSize()));
      mesg.SerializeToArray(read_buffer_.data(), static_cast<int>(read_buffer_.size()));

      // BOOST_LOG_TRIVIAL(info) << this << " Will async_write_until now";
      ba::async_write(sock_,
                      ba::buffer(read_buffer_),
                      std::bind(&ClientSession::on_write, shared_from_this(), _1, _2));
  }

  const std::vector<uint8_t> &ClientSession::get_buffer() {
      return read_buffer_;
  }

  void ClientSession::set_state(std::unique_ptr<ClientState> new_state) {
      state_ = std::move(new_state);
  }

  const ClientState &ClientSession::state() const {
      return *state_;
  }

} // namespace elegram::server
