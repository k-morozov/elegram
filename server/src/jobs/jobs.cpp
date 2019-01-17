#include "jobs.h"
#include "../proto/messages.pb.h"

#include <boost/log/trivial.hpp>

namespace elegram {
  server::ParseCommandJob::ParseCommandJob(std::shared_ptr<server::ClientSession> session)
      : session_(std::move(session)) {}

  /**
   * Determine concrete message, generate new job.
   */
  void server::ParseCommandJob::operator()() {
      BOOST_LOG_TRIVIAL(info) << " ParseCommandJob will run now";

      WrappedMessage message;
      bool readed = message.ParseFromArray(session_->get_buffer().data(),
                                           static_cast<int>(session_->get_buffer().size()));
      if (readed) {
          if (message.has_request()) {
              // go through all requests
              std::unique_ptr<Request> request{message.release_request()};

              if (request->has_register_request()) {
                  ba::post(*session_->job_pool(),
                           RegisterRequestJob(request->release_register_request(), session_));
              } else if (request->has_login_request()) {
                  ba::post(*session_->job_pool(),
                           LoginRequestJob(request->release_login_request(), session_));
              } else if (request->has_chats_request()) {
                  ba::post(*session_->job_pool(),
                           ChatsRequestJob(session_));
              } else if (request->has_contacts_request()) {
                  ba::post(*session_->job_pool(),
                           ContactsRequestJob(session_));
              } else if (request->has_messages_request()) {
                  ba::post(*session_->job_pool(),
                           MessagesRequestJob(request->release_messages_request(), session_));
              } else if (request->has_send_mesg_request()) {
                  ba::post(*session_->job_pool(),
                           SendMesgRequestJob(request->release_send_mesg_request(), session_));
              } else {
                  BOOST_LOG_TRIVIAL(info) << " unsupported request type";
                  session_->stop();
                  return;
              }
              BOOST_LOG_TRIVIAL(info) << " ParseCommandJob will done";
              return;
          } else { // message.has_response()
              BOOST_LOG_TRIVIAL(error) << " got response message instead fo request";
              session_->stop();
              return;
          }
      } else {
          BOOST_LOG_TRIVIAL(error) << " parse error";
          session_->stop();
          return;
      }
  }

  /**
   * Protobuf Requests
   * ----------------------------------------------------------------------------------------------------
   */
  server::RegisterRequestJob::RegisterRequestJob(RegisterRequest *mesg,
                                                 std::shared_ptr<server::ClientSession> session)
      : session_(std::move(session)), mesg_(mesg) {}

  void server::RegisterRequestJob::operator()() {
      // todo make async ?
      bool register_res = session_->storage()->registration(mesg_->name(),
                                                            mesg_->email(),
                                                            mesg_->password());

      std::unique_ptr<StatusResponse> status_resp = std::make_unique<StatusResponse>();
      if (register_res) {
          status_resp->set_result(StatusResponse_RESULT::StatusResponse_RESULT_ACCEPTED);
      } else {
          status_resp->set_result(StatusResponse_RESULT::StatusResponse_RESULT_REJECTED);
      }
      std::unique_ptr<Response> response = std::make_unique<Response>();
      response->set_allocated_status_response(status_resp.release());

      WrappedMessage wrappedMessage;
      wrappedMessage.set_allocated_response(response.release());
      session_->write(wrappedMessage);
  }

  /*-------------------------------------------------------------------------*/
  server::LoginRequestJob::LoginRequestJob(LoginRequest *mesg,
                                           std::shared_ptr<server::ClientSession> session)
      : session_(std::move(session)), mesg_(mesg) {}

  void server::LoginRequestJob::operator()() {
      // todo make async ?3
      std::optional<uint64_t> register_res = session_->storage()->login(mesg_->name(),
                                                                        mesg_->password());

      std::unique_ptr<StatusResponse> status_resp = std::make_unique<StatusResponse>();
      if (register_res.has_value()) {
          status_resp->set_result(StatusResponse_RESULT::StatusResponse_RESULT_ACCEPTED);
          session_->set_state(std::make_unique<ClientState>(register_res.value()));
      } else {
          status_resp->set_result(StatusResponse_RESULT::StatusResponse_RESULT_REJECTED);
      }
      std::unique_ptr<Response> response = std::make_unique<Response>();
      response->set_allocated_status_response(status_resp.release());

      WrappedMessage wrappedMessage;
      wrappedMessage.set_allocated_response(response.release());
      session_->write(wrappedMessage);
  }

  /*-------------------------------------------------------------------------*/
  server::ChatsRequestJob::ChatsRequestJob(std::shared_ptr<server::ClientSession> session)
      : session_(std::move(session)) {}

  void server::ChatsRequestJob::operator()() {
      // todo make async ?
      uint64_t user_id = session_->state().user_id();
      std::unique_ptr<ChatsResponse> chats = session_->storage()->get_chats(user_id);

      std::unique_ptr<Response> response = std::make_unique<Response>();
      response->set_allocated_chats_response(chats.release());

      WrappedMessage wrappedMessage;
      wrappedMessage.set_allocated_response(response.release());
      session_->write(wrappedMessage);
  }

  /*-------------------------------------------------------------------------*/
  server::ContactsRequestJob::ContactsRequestJob(std::shared_ptr<server::ClientSession> session)
      : session_(std::move(session)) {}

  void server::ContactsRequestJob::operator()() {
      // todo make async ?
      uint64_t user_id = session_->state().user_id();
      std::unique_ptr<ContactsResponse> contacts = session_->storage()->get_contacts(user_id);

      std::unique_ptr<Response> response = std::make_unique<Response>();
      response->set_allocated_contacts_response(contacts.release());

      WrappedMessage wrappedMessage;
      wrappedMessage.set_allocated_response(response.release());
      session_->write(wrappedMessage);
  }

  /*-------------------------------------------------------------------------*/
  server::MessagesRequestJob::MessagesRequestJob(MessagesRequest *mesg,
                                                 std::shared_ptr<server::ClientSession> session)
      : session_(std::move(session)), mesg_(mesg) {}

  void server::MessagesRequestJob::operator()() {
      // todo make async ?
      std::unique_ptr<MessagesResponse> messages = session_->storage()->get_messages(mesg_->chat_id());

      std::unique_ptr<Response> response = std::make_unique<Response>();
      response->set_allocated_messages_response(messages.release());

      WrappedMessage wrappedMessage;
      wrappedMessage.set_allocated_response(response.release());
      session_->write(wrappedMessage);
  }

  /*-------------------------------------------------------------------------*/
  server::SendMesgRequestJob::SendMesgRequestJob(SendMessageRequest *mesg,
                                                 std::shared_ptr<server::ClientSession> session)
      : session_(std::move(session)), mesg_(mesg) {}

  void server::SendMesgRequestJob::operator()() {
      // todo make async ?
      bool send_res = session_->storage()->send_message(mesg_->mesg());

      std::unique_ptr<StatusResponse> status_resp = std::make_unique<StatusResponse>();
      if (send_res) {
          status_resp->set_result(StatusResponse_RESULT::StatusResponse_RESULT_ACCEPTED);
      } else {
          status_resp->set_result(StatusResponse_RESULT::StatusResponse_RESULT_REJECTED);
      }

      std::unique_ptr<Response> response = std::make_unique<Response>();
      response->set_allocated_status_response(status_resp.release());

      WrappedMessage wrappedMessage;
      wrappedMessage.set_allocated_response(response.release());
      session_->write(wrappedMessage);
  }

} // namespace elegram
