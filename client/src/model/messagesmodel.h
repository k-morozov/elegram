#ifndef MESSAGESMODEL_H
#define MESSAGESMODEL_H

#include <QAbstractListModel>
#include "messageslist.h"

namespace elegram {
  class MessagesModel : public QAbstractListModel {
   Q_OBJECT
    Q_PROPERTY(MessagesList *list
                   READ
                   list
                   WRITE
                   setList /* NOTIFY listChanged */ )
   public:
    explicit MessagesModel(QObject *parent = nullptr);

    enum {
      SenderIdRole = Qt::UserRole,
      ChatIdRole
    };

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QHash<int, QByteArray> roleNames() const override;

    MessagesList *list() const;
    void setList(MessagesList *list);

   private:
    MessagesList *m_list = nullptr;
  };
} // namespace elegram
#endif // MESSAGESMODEL_H
