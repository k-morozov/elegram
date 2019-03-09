#ifndef CHATSLIST_H
#define CHATSLIST_H

#include <QObject>
#include <QVector>
#include <memory>

namespace elegram {
  struct ChatItem {
    quint64 id;
    QString title;
  };

  class ChatsList : public QObject {
   Q_OBJECT
   public:
    explicit ChatsList(QObject *parent = Q_NULLPTR);

    const ChatItem &at(int index) const;
    int size() const;
    void reset();

   signals:
    void preItemsSetted(int size);
    void postItemsSetted();

    void preListCleared();
    void postListCleared();

   public slots:
    void setItems(QVector<ChatItem> *vec); // unforunately, can't use move semantics here =(

   private:
    std::unique_ptr<QVector<ChatItem>> m_items = std::make_unique<QVector<ChatItem>>();
  };
} // namespace elegram

#endif // CHATSLIST_H
