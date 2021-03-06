#include "jobs.h"
#include "../proto/messages.pb.h"

#include <boost/log/trivial.hpp>
#include <iostream>

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
              } else if (request->has_add_contact_request()) {
                  ba::post(*session_->job_pool(),
                           AddContactRequestJob(request->release_add_contact_request(), session_));
              } else if (request->has_create_chat_request()) {
                  ba::post(*session_->job_pool(),
                           CreateChatRequestJob(request->release_create_chat_request(), session_));
              } else {
                  BOOST_LOG_TRIVIAL(error) << " unsupported request type";
                  session_->stop();
                  return;
              }
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
      bool register_res = session_->storage_connection()->registration(mesg_->name(),
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

      BOOST_LOG_TRIVIAL(info) << " RegisterRequestJob::operator()() done";
  }

  /*-------------------------------------------------------------------------*/
  server::LoginRequestJob::LoginRequestJob(LoginRequest *mesg,
                                           std::shared_ptr<server::ClientSession> session)
      : session_(std::move(session)), mesg_(mesg) {}

  void server::LoginRequestJob::operator()() {
      // todo make async ?3
      std::unique_ptr<LoginResponse> login_response = std::make_unique<LoginResponse>();
      try {
          auto[user_id, user_name] = session_->storage_connection()->login(mesg_->email(), mesg_->password());
          session_->set_state(std::make_unique<ClientState>(user_id, user_name, mesg_->email()));

          std::unique_ptr<Contact> profile = std::make_unique<Contact>();
          profile->set_user_id(session_->state().user_id());
          profile->set_name(session_->state().user_name());
          profile->set_email(session_->state().email());

          login_response->set_allocated_profile(profile.release());
      } catch (const std::invalid_argument &e) {
          // BOOST_LOG_TRIVIAL(error) << e.what();
          // REJECTED
      } catch (...) {
          BOOST_LOG_TRIVIAL(error) << "THROWED SOMETHING UNHANDLED";
      }

      std::unique_ptr<Response> response = std::make_unique<Response>();
      response->set_allocated_login_response(login_response.release());

      WrappedMessage wrappedMessage;
      wrappedMessage.set_allocated_response(response.release());
      session_->write(wrappedMessage);

      BOOST_LOG_TRIVIAL(info) << "LoginRequestJob::operator()() done";
  }

  /*-------------------------------------------------------------------------*/
  server::ChatsRequestJob::ChatsRequestJob(std::shared_ptr<server::ClientSession> session)
      : session_(std::move(session)) {}

  void server::ChatsRequestJob::operator()() {
      // todo make async ?
      if (session_->logged_in()) {
          uint64_t user_id = session_->state().user_id();
          std::unique_ptr<ChatsResponse> chats = session_->storage_connection()->get_chats(user_id);

          std::unique_ptr<Response> response = std::make_unique<Response>();
          response->set_allocated_chats_response(chats.release());

          WrappedMessage wrappedMessage;
          wrappedMessage.set_allocated_response(response.release());
          session_->write(wrappedMessage);
      } else {
          // send empty response
          std::unique_ptr<Response> response = std::make_unique<Response>();
          WrappedMessage wrappedMessage;
          wrappedMessage.set_allocated_response(response.release());
          session_->write(wrappedMessage);
      }

      BOOST_LOG_TRIVIAL(info) << " ChatsRequestJob::operator()() done";
  }

  /*-------------------------------------------------------------------------*/
  server::ContactsRequestJob::ContactsRequestJob(std::shared_ptr<server::ClientSession> session)
      : session_(std::move(session)) {}

  void server::ContactsRequestJob::operator()() {
      // todo make async ?
      if (session_->logged_in()) {
          uint64_t user_id = session_->state().user_id();
          std::unique_ptr<ContactsResponse> contacts = session_->storage_connection()->get_contacts(user_id);

          std::unique_ptr<Response> response = std::make_unique<Response>();
          response->set_allocated_contacts_response(contacts.release());

          WrappedMessage wrappedMessage;
          wrappedMessage.set_allocated_response(response.release());
          session_->write(wrappedMessage);
      } else {
          // send empty response
          std::unique_ptr<Response> response = std::make_unique<Response>();
          WrappedMessage wrappedMessage;
          wrappedMessage.set_allocated_response(response.release());
          session_->write(wrappedMessage);
      }
      BOOST_LOG_TRIVIAL(info) << " ContactsRequestJob::operator()() done";
  }

  /*-------------------------------------------------------------------------*/
  server::MessagesRequestJob::MessagesRequestJob(MessagesRequest *mesg,
                                                 std::shared_ptr<server::ClientSession> session)
      : session_(std::move(session)), mesg_(mesg) {}

  void server::MessagesRequestJob::operator()() {
      // todo make async ?
      if (session_->logged_in()) {
          std::unique_ptr<MessagesResponse>
              messages = session_->storage_connection()->get_messages(mesg_->chat_id());

          std::unique_ptr<Response> response = std::make_unique<Response>();
          response->set_allocated_messages_response(messages.release());

          WrappedMessage wrappedMessage;
          wrappedMessage.set_allocated_response(response.release());
          session_->write(wrappedMessage);
      } else {
          // send empty response
          std::unique_ptr<Response> response = std::make_unique<Response>();
          WrappedMessage wrappedMessage;
          wrappedMessage.set_allocated_response(response.release());
          session_->write(wrappedMessage);
      }
      BOOST_LOG_TRIVIAL(info) << "MessagesRequestJob::operator()() done";
  }

  /*-------------------------------------------------------------------------*/
  server::SendMesgRequestJob::SendMesgRequestJob(SendMessageRequest *mesg,
                                                 std::shared_ptr<server::ClientSession> session)
      : session_(std::move(session)), mesg_(mesg) {}

  void server::SendMesgRequestJob::operator()() {
      if (session_->logged_in()) {
          // todo make async ?
          bool send_res = session_->storage_connection()->send_message(session_->state().user_id(),
                                                                       mesg_->mesg());

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
      } else {
          // send empty response
          std::unique_ptr<Response> response = std::make_unique<Response>();
          WrappedMessage wrappedMessage;
          wrappedMessage.set_allocated_response(response.release());
          session_->write(wrappedMessage);
      }
      BOOST_LOG_TRIVIAL(info) << "SendMesgRequestJob::operator()() done";
  }

  /*-------------------------------------------------------------------------*/
  server::AddContactRequestJob::AddContactRequestJob(AddContactRequest *mesg,
                                                     std::shared_ptr<server::ClientSession> session)
      : session_(std::move(session)), mesg_(mesg) {}

  void server::AddContactRequestJob::operator()() {
      if (session_->logged_in()) {
          bool send_res = session_->storage_connection()->add_contact(session_->state().user_id(),
                                                                      mesg_->email());

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
      } else {
          // send empty response
          std::unique_ptr<Response> response = std::make_unique<Response>();
          WrappedMessage wrappedMessage;
          wrappedMessage.set_allocated_response(response.release());
          session_->write(wrappedMessage);
      }
      BOOST_LOG_TRIVIAL(info) << "AddContactRequestJob::operator()() done";
  }

  /*-------------------------------------------------------------------------*/
  server::CreateChatRequestJob::CreateChatRequestJob(CreateChatRequest *mesg, std::shared_ptr<ClientSession> session)
      : session_(std::move(session)), mesg_(mesg) {}

  void server::CreateChatRequestJob::operator()() {
      if (session_->logged_in()) {
          bool is_created = session_->storage_connection()->create_chat(session_->state().user_id(),
                                                                        session_->state().user_name(),
                                                                        mesg_->friend_id());

          std::unique_ptr<StatusResponse> status_resp = std::make_unique<StatusResponse>();
          if (is_created) {
              status_resp->set_result(StatusResponse_RESULT::StatusResponse_RESULT_ACCEPTED);
          } else {
              status_resp->set_result(StatusResponse_RESULT::StatusResponse_RESULT_REJECTED);
          }

          std::unique_ptr<Response> response = std::make_unique<Response>();
          response->set_allocated_status_response(status_resp.release());

          WrappedMessage wrappedMessage;
          wrappedMessage.set_allocated_response(response.release());
          session_->write(wrappedMessage);
      } else {
          // send empty response
          std::unique_ptr<Response> response = std::make_unique<Response>();
          WrappedMessage wrappedMessage;
          wrappedMessage.set_allocated_response(response.release());
          session_->write(wrappedMessage);
      }
      BOOST_LOG_TRIVIAL(info) << "CreateChatRequestJob::operator()() done";
  }
} // namespace elegram
