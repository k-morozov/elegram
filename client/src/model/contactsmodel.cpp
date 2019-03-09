#include "contactsmodel.h"

namespace elegram {
  ContactsModel::ContactsModel(QObject *parent)
      : QAbstractListModel(parent) {
  }

  int ContactsModel::rowCount(const QModelIndex &parent) const {
      // For list models only the root node (an invalid parent) should return the list's size. For all
      // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
      if (parent.isValid() || !m_list)
          return 0;

      return m_list->size();
  }

  QVariant ContactsModel::data(const QModelIndex &index, int role) const {
      if (!index.isValid() || !m_list)
          return QVariant();

      const ContactItem &item = m_list->at(index.row());
      switch (role) {
          case UserIdRole:return QVariant(item.user_id);
          case NameRole:return QVariant(item.name);
          case EmailRole:return QVariant(item.email);
      }

      return QVariant();
  }

  QHash<int, QByteArray> ContactsModel::roleNames() const {
      QHash<int, QByteArray> names;
      names[UserIdRole] = "user_id";
      names[NameRole] = "name";
      names[EmailRole] = "email";
      return names;
  }

  ContactsList *ContactsModel::list() const {
      return m_list;
  }

  void ContactsModel::setList(ContactsList *list) {
      beginResetModel();

      if (m_list) {
          m_list->disconnect(this);
      }
      m_list = list;

      if (m_list) {
          // connect m_list's signals with functions that should be called
          // before and after insertion new node
          connect(m_list, &ContactsList::preItemsSetted, this, [=](int size) {
              beginInsertRows(QModelIndex(), 0, size - 1);
          });
          connect(m_list, &ContactsList::postItemsSetted, this, [=]() {
              endInsertRows();
          });

          connect(m_list, &ContactsList::preListCleared, this, [=]() {
              const int last = m_list->size();
              beginRemoveRows(QModelIndex(), 0, last - 1);
          });
          connect(m_list, &ContactsList::postListCleared, this, [=]() {
              endRemoveRows();
          });
      }

      endResetModel();
  }

} // namespace elegram
