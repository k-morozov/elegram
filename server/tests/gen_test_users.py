# encoding: utf8

import psycopg2 as pg_driver
from Crypto.Hash import SHA256

db = None
cur = None

try:
    db = pg_driver.connect(user='postgres')

    cur = db.cursor()

    guys = [
        ("John", "john@mail.ru", "12345678"),
        ("Jimm", "jimm@jimm.com", "hatejohn"),
        ("Jack", "ja@ck.com", "jack37"),
        ("Vasya", "vas@ya.com", "samtivasya")
    ]

    # Вставляем в таблицу Clients 4 пользователей
    for name, email, password in guys:
        cmd = "INSERT INTO Client (name, email, password_hash) " \
              "VALUES ('{0}', '{1}', {2} )". \
            format(name, email, pg_driver.Binary(SHA256.new(password.encode('utf-8')).digest()))
        cur.execute(cmd)
        print(cmd)


    def connect(id1, id2):
        '''
        Make id2 a friend of id1, one direction link
        '''
        cmd1 = "INSERT INTO ClientToContact(client_id, friend_id) " \
               "VALUES ({0}, {1})".format(id1, id2)
        cur.execute(cmd1)
        print(cmd1)


    def make_friends(id1, id2):
        connect(id1, id2)
        connect(id2, id1)


    make_friends(1, 2)
    make_friends(1, 3)
    make_friends(3, 2)

    db.commit()


    # create some chats between some of friends
    def create_chat():
        cmd = "INSERT INTO Chat DEFAULT VALUES RETURNING id "
        cur.execute(cmd)
        print(cmd)
        row = cur.fetchone()
        print("RETURNED new created chat_id = {0}".format(row[0]))
        return row[0]


    chat_id1 = create_chat()  # 1
    chat_id2 = create_chat()  # 2
    db.commit()


    def connect_to_chat(chat_id, client_id, title):
        cmd = "INSERT INTO ClientToChat(client_id, chat_id, title) VALUES({0}, {1}, '{2}')".format(client_id, chat_id,
                                                                                                   title)
        cur.execute(cmd)
        print(cmd)


    # chat 1: 1 <-> 2
    connect_to_chat(chat_id1, 1, "Chat with my friend Jimmy =)")
    connect_to_chat(chat_id1, 2, "John and his problems =(")

    # chat 2: 1 <-> 3
    connect_to_chat(chat_id2, 1, "Chat with Jack")
    connect_to_chat(chat_id2, 3, "Chat with John")


    def add_message(from_id: int, chat_id: int, mesg: str):
        cmd = "INSERT INTO Message(sender_id, chat_id, mesg) " \
              "VALUES ({0}, {1}, '{2}')".format(from_id, chat_id, mesg)
        cur.execute(cmd)
        print(cmd)


    add_message(1, 1, "Hello, jimm!")
    add_message(2, 1, "Hello, john!")
    add_message(1, 1, "How are u?")
    add_message(2, 1, "Fine, tnks!")
    add_message(2, 1, "And what about u")
    add_message(1, 1, "Nice ;)")

    add_message(2, 2, "Hello, johny!")
    add_message(1, 2, "Hello, jacky!")

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
