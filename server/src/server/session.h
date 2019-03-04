#pragma once

#include "../storage/abstract_storage.h"
#include "../proto/messages.pb.h"

#include <boost/asio.hpp>
#include <memory>
#include <vector>

namespace elegram::server {
  namespace ba = boost::asio;

  class ClientState {
   public:
    explicit ClientState(uint64_t user_id);
    uint64_t user_id() const;

   private:
    uint64_t user_id_;
  };

  class ClientSession : public std::enable_shared_from_this<ClientSession> {
   public:
    ClientSession(ba::io_service &service,
                  std::shared_ptr<ba::thread_pool> job_pool,
                  std::shared_ptr<AbstractStorageConnection> storage_conn);
    ~ClientSession();

    ba::ip::tcp::socket &sock();
    const std::shared_ptr<ba::thread_pool> &job_pool();
    const std::shared_ptr<AbstractStorageConnection> &storage_connection();
    void set_state(std::unique_ptr<ClientState> &&new_state);
    const ClientState &state() const;

    const std::vector<uint8_t> &get_buffer();
    void start();
    void stop();
    void write(const WrappedMessage &mesg);

   private:
    using error_code = boost::system::error_code;

    // every incoming packet contain two parts:
    //   -  length prefix
    //   -  protobuf message
    void on_read_prefix(const error_code &err, size_t bytes);
    void do_async_read_prefix();
    void on_read_message(const error_code &err, size_t readed_bytes, size_t mesg_size);
    void do_async_read_message(size_t mesg_size);

    void do_async_write(const WrappedMessage &mesg);
    void on_write(const error_code &err, size_t bytes);

   private:
    ba::io_service &service_;
    ba::ip::tcp::socket sock_;
    bool stopped_ = true;
    constexpr static uint64_t PREFIX_SIZE = sizeof(uint64_t) + 1; // fixed64 in protobuf

    // ba::streambuf read_buffer_;
    std::unique_ptr<ClientState> state_ = nullptr;
    std::vector<uint8_t> read_buffer_{};

    std::shared_ptr<ba::thread_pool> job_pool_;
    std::shared_ptr<AbstractStorageConnection> storage_conn_;
  };
} // namespace elegram::server
