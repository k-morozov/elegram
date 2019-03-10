#ifndef NETWORK_SESSION_H
#define NETWORK_SESSION_H
#include <QtNetwork>
#include <QObject>
#include <QIntValidator>

#include <memory>
#include <vector>
#include <functional>

#include "../jobs/jobs.h"

namespace elegram {
  class Backend;

  class Session : public QObject {
   Q_OBJECT
   public:
    explicit Session(Backend *backend, QObject *parent = Q_NULLPTR);

    /**
     * @brief request
     * @param request
     * Send request to socket if it is connected.
     */
    void request(std::unique_ptr<elegram::Request> &&request,
                 std::function<void(std::unique_ptr<Response> &&response)> &&callback);
    /**
     *
     * @return QVariant<QString> in case of error, string contain explanation of error
     *          otherwise return empty QVariant
     */
    QVariant connect(const QString &address, const QString &port);
    bool isConnected() const;
   private:

    qint64 readPrefix();
    std::unique_ptr<WrappedMessage> readMessage(qint64 mesg_size);
    void processResponse(std::unique_ptr<WrappedMessage> &&message);

   private slots:
    void onReadyRead();
    void onError(QAbstractSocket::SocketError socketError);
    void onConnection();
    void onDisconnection();

   private:
    std::unique_ptr<QTcpSocket> m_sock = std::make_unique<QTcpSocket>();
    Backend *m_backend;

    QIntValidator m_portValidator{0, 65535};
    QVector<char> m_buffer{};
    std::function<void(std::unique_ptr<Response> &&response)> m_callback;

    const qint64 PREFIX_SIZE = sizeof(uint64_t) + 1; // fixed64 in protobuf;
    qint64 m_bytesReaded = 0;
    qint64 m_messageSize = 0;
  };

} // namespace elegram

#endif // NETWORK_SESSION_H
