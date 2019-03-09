#ifndef CHATSMODEL_H
#define CHATSMODEL_H

#include <QAbstractListModel>
#include "chatslist.h"

namespace elegram {
  class ChatsModel : public QAbstractListModel {
   Q_OBJECT
    Q_PROPERTY(ChatsList *list
                   READ
                   list
                   WRITE
                   setList /* NOTIFY listChanged */ )
   public:
    explicit ChatsModel(QObject *parent = nullptr);

    enum {
      ChatIdRole = Qt::UserRole
    };

    // Basic functionality:
    int rowCount(const QModelIndex &parent) const override;

    QVariant data(const QModelIndex &index, int role) const override;

    QHash<int, QByteArray> roleNames() const override;

    ChatsList *list() const;
    void setList(ChatsList *list);

   private:
    ChatsList *m_list = nullptr;
  };
} // namespace elegram


#endif // CHATSMODEL_H
