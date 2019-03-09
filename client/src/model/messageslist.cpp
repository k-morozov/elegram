#include "messageslist.h"
#include <QDebug>

namespace elegram {
  MessagesList::MessagesList(QObject *parent)
      : QObject(parent) {
  }

  const MessageItem &MessagesList::at(int index) const {
      assert(m_items);
      return m_items->at(index);
  }

  int MessagesList::size() const {
      return m_items ? m_items->size() : 0;
  }

  quint64 MessagesList::currentChatId() const {
      return m_currentChatId;
  }

  void MessagesList::setCurrentChatId(const quint64 &chatId) {
      if (m_currentChatId != chatId) {
          // say view to remove all previous items
          emit preListCleared(size());
          m_items = nullptr;
          emit postListCleared();

          // and load items for other chat_id
          m_currentChatId = chatId;
          auto it = m_mapToItems.find(m_currentChatId);
          if (it != m_mapToItems.end()) {
              emit preItemsSetted((*it)->size());
              m_items = *it;
              emit postItemsSetted();
          }
      }
  }

  void MessagesList::setItems(QVector<MessageItem> *vec) {
      // say view to remove all previous items
      if (m_items) {
          emit preListCleared(size());
          m_items = nullptr;
          emit postListCleared();
      }

      // ans set new items
      if (vec) {
          emit preItemsSetted(vec->size());
          m_mapToItems[m_currentChatId] = std::shared_ptr<QVector<MessageItem>>(vec);
          m_items = m_mapToItems[m_currentChatId];
          emit postItemsSetted();
          qDebug() << "[MessagesList] setItems() done with size = " << size() << ", chat_id = " << m_currentChatId;
      }
  }

} // namespace elegram

