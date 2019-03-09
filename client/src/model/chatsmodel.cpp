#include "chatsmodel.h"

namespace elegram {

  ChatsModel::ChatsModel(QObject *parent)
      : QAbstractListModel(parent) {
  }

  int ChatsModel::rowCount(const QModelIndex &parent) const {
      // For list models only the root node (an invalid parent) should return the list's size. For all
      // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
      if (parent.isValid() || !m_list)
          return 0;

      return m_list->size();
  }

  QVariant ChatsModel::data(const QModelIndex &index, int role) const {
      if (!index.isValid() || !m_list)
          return QVariant();

      const ChatItem &item = m_list->at(index.row());
      switch (role) {
          case ChatIdRole:return QVariant(item.id);
          case Qt::DisplayRole:return QVariant(item.title);
          default:break;
      }

      return QVariant();
  }

  QHash<int, QByteArray> ChatsModel::roleNames() const {
      QHash<int, QByteArray> names;
      names[ChatIdRole] = "chat_id";
      names[Qt::DisplayRole] = "chat_title";
      return names;
  }

  ChatsList *ChatsModel::list() const {
      return m_list;
  }

  void ChatsModel::setList(ChatsList *list) {
      beginResetModel();

      if (m_list) {
          m_list->disconnect(this);
      }
      m_list = list;

      if (m_list) {
          // connect m_list's signals with functions that should be called
          // before and after insertion new node
          connect(m_list, &ChatsList::preItemsSetted, this, [=](int size) {
              beginInsertRows(QModelIndex(), 0, size - 1);
          });
          connect(m_list, &ChatsList::postItemsSetted, this, [=]() {
              endInsertRows();
          });

          connect(m_list, &ChatsList::preListCleared, this, [=]() {
              const int last = m_list->size();
              beginRemoveRows(QModelIndex(), 0, last - 1);
          });
          connect(m_list, &ChatsList::postListCleared, this, [=]() {
              endRemoveRows();
          });
      }

      endResetModel();
  }
} // namespace elegram

