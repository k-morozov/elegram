#include "messagesmodel.h"
#include <QDebug>

namespace elegram {
  MessagesModel::MessagesModel(QObject *parent)
      : QAbstractListModel(parent) {
  }

  int MessagesModel::rowCount(const QModelIndex &parent) const {
      // For list models only the root node (an invalid parent) should return the list's size. For all
      // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
      if (parent.isValid() || !m_list)
          return 0;

      return m_list->size();
  }

  QVariant MessagesModel::data(const QModelIndex &index, int role) const {
      if (!index.isValid() || !m_list)
          return QVariant();

      const MessageItem &item = m_list->at(index.row());
      switch (role) {
          case SenderIdRole:return QVariant(item.sender_id);
          case Qt::DisplayRole:return QVariant(item.text);
      }

      return QVariant();
  }

  QHash<int, QByteArray> MessagesModel::roleNames() const {
      QHash<int, QByteArray> names;
      names[SenderIdRole] = "sender_id";
      names[Qt::DisplayRole] = "mesg";
      return names;
  }

  MessagesList *MessagesModel::list() const {
      return m_list;
  }

  void MessagesModel::setList(MessagesList *list) {
      beginResetModel();

      if (m_list) {
          m_list->disconnect(this);
      }
      m_list = list;

      if (m_list) {
          // connect m_list's signals with functions that should be called
          // before and after insertion new node
          connect(m_list, &MessagesList::preItemsSetted, this, [=](int size) {
              beginInsertRows(QModelIndex(), 0, size - 1);
          });
          connect(m_list, &MessagesList::postItemsSetted, this, [=]() {
              endInsertRows();
          });

          connect(m_list, &MessagesList::preListCleared, this, [=](int size) {
              beginRemoveRows(QModelIndex(), 0, size - 1);
          });
          connect(m_list, &MessagesList::postListCleared, this, [=]() {
              endRemoveRows();
          });
      }
      endResetModel();
  }
} // namespace elegram
