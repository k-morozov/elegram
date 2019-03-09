#include "chatslist.h"

namespace elegram {
  ChatsList::ChatsList(QObject *parent)
      : QObject(parent) {
  }

  const ChatItem &ChatsList::at(int index) const {
      return m_items->at(index);
  }

  int ChatsList::size() const {
      return m_items->size();
  }

  void ChatsList::reset() {
      if (!m_items->empty()) {
          emit preListCleared();
          m_items->clear();
          emit postListCleared();
      }
  }

  void ChatsList::setItems(QVector<ChatItem> *vec) {
      // say view to remove all previous items
      reset();

      // ans set new items
      if (vec) {
          emit preItemsSetted(vec->size());
          m_items.reset(vec);
          emit postItemsSetted();
      }
  }

} // namespace elegram
