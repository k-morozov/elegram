#ifndef CONTACTSLIST_H
#define CONTACTSLIST_H

#include <QObject>
#include <QVector>
#include <QHash>
#include <memory>

namespace elegram {
  struct ContactItem {
    quint64 user_id;
    QString name;
    QString email;
  };

  class ContactsList : public QObject {
   Q_OBJECT
   public:
    explicit ContactsList(QObject *parent = Q_NULLPTR);

    const ContactItem &at(int index) const;
    int size() const;
    void reset();
    QString getUserBy(quint64 sender_id) const;

   signals:
    void preItemsSetted(int size);
    void postItemsSetted();
    void preListCleared();
    void postListCleared();

   public slots:
    //    void appendItem(const ContactItem &item); // unforunately, can't use move semantics here =(
    void setItems(QVector<ContactItem> *vec); // unforunately, can't use move semantics here =(

   private:
    std::unique_ptr<QVector<ContactItem>> m_items = std::make_unique<QVector<ContactItem>>();
    QHash<quint64, const ContactItem *> m_idToItem{};
  };
} // namespace elegram

#endif // CONTACTSLIST_H
