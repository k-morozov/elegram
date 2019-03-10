#ifndef JOBS_H
#define JOBS_H

#include "ijob.h"

#include <QString>
#include <memory>

namespace elegram {
  class LoginJob : public IJob {
   public:
    explicit LoginJob(Backend *backend);
    void exec(const QString &email, const QString &password);
    void responseProcessingCallback(std::unique_ptr<Response> &&response) override;
   private:
    Backend *m_backend;
  };

  class RegisterJob : public IJob {
   public:
    explicit RegisterJob(Backend *backend);
    void exec(const QString &name, const QString &email, const QString &password);
    void responseProcessingCallback(std::unique_ptr<Response> &&response) override;
   private:
    Backend *m_backend;
  };

  class LoadChatsJob : public IJob {
   public:
    explicit LoadChatsJob(Backend *backend);
    void exec();
    void responseProcessingCallback(std::unique_ptr<Response> &&response) override;
   private:
    Backend *m_backend;
  };

  class LoadContactsJob : public IJob {
   public:
    explicit LoadContactsJob(Backend *backend);
    void exec();
    void responseProcessingCallback(std::unique_ptr<Response> &&response) override;
   private:
    Backend *m_backend;
  };

  class LoadMessagesJob : public IJob {
   public:
    explicit LoadMessagesJob(Backend *backend);
    void exec(quint64 chatId);
    void responseProcessingCallback(std::unique_ptr<Response> &&response) override;
   private:
    Backend *m_backend;
  };

  class SendMessageJob : public IJob {
   public:
    explicit SendMessageJob(Backend *backend);
    void exec(quint64 chatId, const QString &message);
    void responseProcessingCallback(std::unique_ptr<Response> &&response) override;
   private:
    Backend *m_backend;
  };

  class AddContactJob : public IJob {
   public:
    explicit AddContactJob(Backend *backend);
    void exec(const QString &email);
    void responseProcessingCallback(std::unique_ptr<Response> &&response) override;
   private:
    Backend *m_backend;
  };

  class LoadDataJob : public IJob {
   public:
    explicit LoadDataJob(Backend *backend);
    void exec();
    void responseProcessingCallback(std::unique_ptr<Response> &&response) override;
   private:
    Backend *m_backend;
  };

  class CreateChatJob : public IJob {
   public:
    explicit CreateChatJob(Backend *backend);
    void exec(quint64 friendId);
    void responseProcessingCallback(std::unique_ptr<Response> &&response) override;
   private:
    Backend *m_backend;
  };
} // namespace elegram

#endif // JOBS_H
