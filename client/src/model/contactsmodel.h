#ifndef CONTACTMODEL_H
#define CONTACTMODEL_H

#include <QAbstractListModel>
#include "contactslist.h"

namespace elegram {
  class ContactsModel : public QAbstractListModel {
   Q_OBJECT
    Q_PROPERTY(ContactsList *list
                   READ
                   list
                   WRITE
                   setList /* NOTIFY listChanged */ )
   public:
    explicit ContactsModel(QObject *parent = nullptr);

    enum {
      UserIdRole = Qt::UserRole,
      NameRole,
      EmailRole
    };

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QHash<int, QByteArray> roleNames() const override;

    ContactsList *list() const;
    void setList(ContactsList *list);

   private:
    ContactsList *m_list = nullptr;
  };
} // namespace elegram
#endif // CONTACTMODEL_H
