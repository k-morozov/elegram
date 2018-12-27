# encoding: utf8
import psycopg2 as pg_driver
from Crypto.Hash import SHA256

db = None
cur = None

try:
    db = pg_driver.connect(user='postgres')

    cur = db.cursor()

    guys = [
        ("john", "john@mail.ru", "12345678"),
        ("jimm", "jimm@jimm.qu", "john_super_guy"),
        ("jasck", "jas@ck.ru", "jasker37"),
        ("vasya", "vas@ya.ru", "samtivasya")
    ]

    # Вставляем в таблицу Clients 4 пользователей
    for name, email, password in guys:
        cmd = "INSERT INTO Client (name, email, password_hash) " \
              "VALUES ('{0}', '{1}', '{2}' )". \
            format(name, password, SHA256.new(password.encode('utf-8')).hexdigest())
        cur.execute(cmd)
        print(cmd)


    def add_friend(id1, id2):
        '''
        Make id2 a friend of id1, one direction link
        '''
        cmd1 = "INSERT INTO ClientToContact(client_id, friend_id) " \
               "VALUES ({0}, {1})".format(id1, id2)
        cur.execute(cmd1)
        print(cmd1)


    add_friend(1, 2)
    add_friend(2, 1)

    add_friend(1, 3)
    add_friend(3, 1)

    add_friend(3, 2)

    db.commit()


    # create some chats between some of friends
    def create_chat():
        cmd = "INSERT INTO Chat DEFAULT VALUES"
        cur.execute(cmd)
        print(cmd)


    create_chat()  # 1
    create_chat()  # 2
    db.commit()


    def connect_to_chat(chat_id, client_id):
        cmd = "INSERT INTO ClientToChat(client_id, chat_id) VALUES({0}, {1})".format(client_id, chat_id)
        cur.execute(cmd)
        print(cmd)


    # chat 1: 1 <-> 2
    connect_to_chat(1, 1)
    connect_to_chat(1, 2)

    # chat 2: 1 <-> 3
    connect_to_chat(2, 1)
    connect_to_chat(2, 3)


    def add_message(from_id: int, chat_id: int, mesg: str):
        cmd = "INSERT INTO Message(sender_id, chat_id, mesg) " \
              "VALUES ({0}, {1}, '{2}')".format(from_id, chat_id, mesg)
        cur.execute(cmd)
        print(cmd)


    add_message(1, 1, "Hello, jimm!")
    add_message(2, 1, "Hello, john!")
    add_message(1, 1, "How are u?")
    add_message(2, 1, "Fine, tnks!")

    add_message(2, 2, "Hello, johny!")
    add_message(1, 2, "Hello, jas!")

    db.commit()

except Exception as error:
    print("# Error while connecting to PostgreSQL", error)
finally:
    # closing database connection.
    if db:
        db.commit()
        cur.close()
        db.close()
        print("# PostgreSQL connection is closed")
