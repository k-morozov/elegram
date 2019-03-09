#include "contactslist.h"

namespace elegram {
  ContactsList::ContactsList(QObject *parent)
      : QObject(parent) {
  }

  const ContactItem &ContactsList::at(int index) const {
      return m_items->at(index);
  }

  int ContactsList::size() const {
      return m_items->size();
  }

  void ContactsList::reset() {
      if (!m_items->empty()) {
          emit preListCleared();
          m_items->clear();
          emit postListCleared();
      }
  }

  QString ContactsList::getUserBy(quint64 sender_id) const {
      auto it = m_idToItem.find(sender_id);
      if (it != m_idToItem.end()) {
          return (*it)->name;
      } else {
          return QString{"Unknown user_id (not found among contacts)"};
      }
  }

  void ContactsList::setItems(QVector<ContactItem> *vec) {
      // say view to remove all previous items
      reset();

      // ans set new items
      if (vec) {
          emit preItemsSetted(vec->size());
          m_items.reset(vec);
          emit postItemsSetted();
      }

      // fill map for fast access to items -- useful for drawing contacts
      m_idToItem.clear();
      for (const auto &item : *m_items) {
          m_idToItem[item.user_id] = &item;
      }
  }
}
