#include <utility>

#include "network_session.h"
#include "src/backend.h"

namespace elegram {
  namespace {
    const qint64 PARSE_ERROR = -1;
  }

  Session::Session(Backend *backend, QObject *parent)
      : QObject(parent), m_backend(backend) {
      QObject::connect(m_sock.get(), &QIODevice::readyRead, this, &Session::onReadyRead);
      QObject::connect(m_sock.get(), QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),
                       this, &Session::onError);
      QObject::connect(m_sock.get(), &QAbstractSocket::connected, this, &Session::onConnection);
      QObject::connect(m_sock.get(), &QAbstractSocket::disconnected, this, &Session::onDisconnection);
  }

  void Session::request(std::unique_ptr<Request> &&request,
                        std::function<void(std::unique_ptr<Response> &&response)> &&callback) {
      if (isConnected()) {
          qDebug() << "[Session::request] Will save callback and make request";
          m_callback = std::move(callback);

          WrappedMessage mesg;
          mesg.set_allocated_request(request.release());

          m_buffer.resize(mesg.ByteSize() + static_cast<int>(PREFIX_SIZE));

          LengthPrefix length_prefix;
          length_prefix.set_length(static_cast<google::protobuf::uint64>(mesg.ByteSize()));

          length_prefix.SerializeToArray(m_buffer.data(), static_cast<int>(PREFIX_SIZE));
          mesg.SerializeToArray(m_buffer.data() + PREFIX_SIZE, mesg.ByteSize());

          m_sock->write(m_buffer.data(), m_buffer.size());
          qDebug() << "[Session::request] Sended " << m_buffer.size() << " bytes, include prefix";
      } else {
          // add connection_error_job
          qDebug() << "[Session::request] Not isConnected, can't send request!";
          emit m_backend->operationAborted("Not connected, can't send requests! Check connection settings.");
      }
  }

  QVariant Session::connect(const QString &address, const QString &port) {
      int pos = 0;
      QHostAddress hostAddress;
      hostAddress.setAddress(address);
      QString portCopy = port;

      if (hostAddress.isNull()) {
          return QVariant::fromValue(QString("Invalid ip address"));
      } else if (m_portValidator.validate(portCopy, pos) != QIntValidator::Acceptable) {
          return QVariant::fromValue(QString("Invalid port"));
      } else {
          m_sock->abort();
          m_sock->connectToHost(address, port.toUShort());
          return QVariant();
      }
  }

  bool Session::isConnected() const {
      return (m_sock->state() == QTcpSocket::ConnectedState);
  }

  /*------------------------------------------------------------------------------------------*/
  qint64 Session::readPrefix() {
      m_buffer.resize(static_cast<int>(PREFIX_SIZE));
      qint64 readed = m_sock->read(m_buffer.data(), PREFIX_SIZE);
      m_bytesReaded = readed;

      LengthPrefix prefix;
      bool parsed = prefix.ParseFromArray(m_buffer.data(), readed);
      if (parsed) {
          return static_cast<qint64>(prefix.length());
      } else {
          return PARSE_ERROR;
      }
  }

  std::unique_ptr<WrappedMessage> Session::readMessage(qint64 mesg_size) {
      m_buffer.resize(static_cast<int>(mesg_size));
      qint64 readed = m_sock->read(m_buffer.data(), mesg_size);
      m_bytesReaded += static_cast<quint64>(readed);

      std::unique_ptr<WrappedMessage> message = std::make_unique<WrappedMessage>();
      bool parsed = message->ParseFromArray(m_buffer.data(), readed);

      if (parsed) {
          return message;
      } else {
          return nullptr;
      }
  }

  void Session::processResponse(std::unique_ptr<WrappedMessage> &&message) {
      if (message->has_response()) {
          m_callback(std::unique_ptr<Response>{message->release_response()});
      } else {
          qWarning("[Session::processResponse] Message has no response");
      }
  }

  /**
   * @brief Connection::onReadMessage
   * Read response and emit signal for processing response
   */
  void Session::onReadyRead() {
      qDebug() << "[Session::onReadyRead()] bytesAvailible now = " << m_sock->bytesAvailable();

      auto load_mesg = [this]() {
          std::unique_ptr<WrappedMessage> readedMesg = readMessage(m_messageSize);
          if (readedMesg) {
              qDebug() << "[Session::onReadyRead()] message loaded succesfully";
              processResponse(std::move(readedMesg));
          } else {
              qWarning("[Session::onReadyRead()] Can't parse message!");
              m_sock->abort();
          }
      };

      // try read preifx
      if (m_bytesReaded < PREFIX_SIZE && m_sock->bytesAvailable() >= PREFIX_SIZE) {
          qWarning("[Session::onReadyRead()] Will try to read prefix now");
          m_bytesReaded = 0;
          m_messageSize = readPrefix();

          if (m_messageSize == PARSE_ERROR) {
              qWarning("[Session::onReadyRead()] Can't parse prefix!");
              m_sock->abort();
              m_bytesReaded = 0;
              return;
          }

          qDebug() << "[Session::onReadyRead()] onReadyRead(): readed prefix, len = " << m_messageSize
                   << ", bytesAvailible now = " << m_sock->bytesAvailable();

          // try to read message
          if (m_sock->bytesAvailable() == m_messageSize) {
              load_mesg();
              m_bytesReaded = 0;
          }

          ///////////////////////////////////////////////////////////////////////////////////////
      } else if (m_bytesReaded == PREFIX_SIZE && m_sock->bytesAvailable() >= m_messageSize) {
          // prefix was already read, prefix was recieved
          qWarning("[Session::onReadyRead()] Will load message now");
          load_mesg();
          m_bytesReaded = 0;
      } else {
          qWarning("[Session::onReadyRead()] Not enoug data recieved");
          m_bytesReaded = 0;
      }

      // else wait new data
  }

  void Session::onError(QAbstractSocket::SocketError socketError) {
      switch (socketError) {
          case QAbstractSocket::RemoteHostClosedError:break;

          case QAbstractSocket::HostNotFoundError: {
              //        QMessageBox::information(this, tr("Elegram Client"),
              //                                 tr("The host was not found. Please check the "
              //                                    "host name and port settings."));
              const QString err = "The host was not found. Please check the "
                                  "host name and port settings.";
              qDebug() << err;
              emit m_backend->operationAborted(err);
              break;
          }

          case QAbstractSocket::ConnectionRefusedError: {
              const QString err = "The connection was refused by the peer. "
                                  "Make sure the Elegram server is running, "
                                  "and check that the host name and port "
                                  "settings are correct.";
              qDebug() << err;
              emit m_backend->operationAborted(err);
              break;
          }

          default: {
              const QString err = QString("The following error occurred: %1.").arg(m_sock->errorString());
              qDebug() << err;
              emit m_backend->operationAborted(err);
              break;
          }
      }
  }

  void Session::onConnection() {
      qDebug() << "[Session::onConnection()] Connected!";
      emit m_backend->showMessage("Connected!");
  }

  void Session::onDisconnection() {
      qDebug() << "[Session::onConnection()] Disconnected!";
      emit m_backend->disconnected();
  }

} // namespace elegram
