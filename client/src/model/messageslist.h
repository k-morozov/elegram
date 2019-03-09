#ifndef MESSAGELIST_H
#define MESSAGELIST_H

#include <QObject>
#include <QMap>
#include <QVector>
#include <memory>

namespace elegram {
  struct MessageItem {
    quint64 sender_id;
    QString text;
  };

  class MessagesList : public QObject {
   Q_OBJECT
   public:
    explicit MessagesList(QObject *parent = Q_NULLPTR);

    const MessageItem &at(int index) const;
    int size() const;

    Q_INVOKABLE quint64 currentChatId() const;
    Q_INVOKABLE void setCurrentChatId(const quint64 &currentChatId);

   signals:
    void preItemsSetted(int size);
    void postItemsSetted();

    void preListCleared(int size);
    void postListCleared();

   public slots:
    void setItems(QVector<MessageItem> *vec); // unforunately, can't use move semantics here =(

   private:
    // shared_ptr, cause QMap whant in be copyable =(
    QMap<quint64, std::shared_ptr<QVector<MessageItem>>> m_mapToItems{}; // chat_id --> QVector<MessageItem>
    quint64 m_currentChatId = 0;
    std::shared_ptr<QVector<MessageItem>> m_items = nullptr;
  };
} // namespace elegram


#endif // MESSAGELIST_H
