#!/usr/bin/python3
import sys
import unittest
from socket import *

from messages_pb2 import *

PREFIX_LENGTH_SIZE = 8 + 1


def make_socket(port=8000):
    """ Create a socket on localhost and return it.
    """
    sockobj = socket(AF_INET, SOCK_STREAM)
    sockobj.connect(('localhost', port))
    return sockobj


def send_message(sock, message):
    """ Send a serialized message (protobuf Message interface)
        to a socket, prepended by its length packed in 4 bytes.
    """
    s = message.SerializeToString()
    length_prefix = LengthPrefix()
    length_prefix.length = len(s)

    # print(length_prefix.ByteSize())
    # assert length_prefix.ByteSize() == 8 + 1

    packed_message = length_prefix.SerializeToString() + s
    sock.send(packed_message)


def socket_read_n(sock, n):
    """ Read exactly n bytes from the socket.
        Raise RuntimeError if the connection closed before n bytes were read.
    """
    buf = b''
    while n > 0:
        data = sock.recv(n)
        if data == '':
            raise RuntimeError('unexpected connection close')
        buf += data
        n -= len(data)
    return buf


def get_response(sock):
    """ Read a serialized response message from a socket.
    """
    len_prefix = LengthPrefix()
    len_buf = socket_read_n(sock, PREFIX_LENGTH_SIZE)
    len_prefix.ParseFromString(len_buf)
    # print("# Response prefix length = ", len_prefix.length)

    msg_buf = socket_read_n(sock, len_prefix.length)
    # print("# msg_buf: ", msg_buf)

    msg = WrappedMessage()
    msg.ParseFromString(msg_buf)

    return msg


def login_request(sock, login, passw):
    login_req = LoginRequest()
    login_req.name = login
    login_req.password = passw

    request = Request()
    request.login_request.MergeFrom(login_req)

    msg = WrappedMessage()
    msg.request.MergeFrom(request)

    send_message(sock, msg)
    return get_response(sock)


def register_request(sock, login, email, passw):
    register_req = RegisterRequest()
    register_req.name = login
    register_req.email = email
    register_req.password = passw

    request = Request()
    request.register_request.MergeFrom(register_req)

    msg = WrappedMessage()
    msg.request.MergeFrom(request)

    send_message(sock, msg)
    return get_response(sock)


def contacts_request(sock):
    contacts_req = AllMyContactsRequest()

    request = Request()
    request.contacts_request.MergeFrom(contacts_req)

    msg = WrappedMessage()
    msg.request.MergeFrom(request)

    send_message(sock, msg)
    return get_response(sock)


def chats_request(sock):
    chats_req = AllMyChatsRequest()

    request = Request()
    request.chats_request.MergeFrom(chats_req)

    msg = WrappedMessage()
    msg.request.MergeFrom(request)

    send_message(sock, msg)
    return get_response(sock)


def messages_request(sock, chat_id):
    messages_req = MessagesRequest()
    messages_req.chat_id = chat_id
    request = Request()
    request.messages_request.MergeFrom(messages_req)

    msg = WrappedMessage()
    msg.request.MergeFrom(request)

    send_message(sock, msg)
    return get_response(sock)


def send_message_request(sock, chat_id, text):
    mesg = MessageToSend()
    mesg.chat_id = chat_id
    mesg.text = text

    send_message_req = SendMessageRequest()
    send_message_req.mesg.MergeFrom(mesg)

    request = Request()
    request.send_mesg_request.MergeFrom(send_message_req)

    msg = WrappedMessage()
    msg.request.MergeFrom(request)

    send_message(sock, msg)
    return get_response(sock)


def connect(port=8000):
    if len(sys.argv) >= 2:
        port = int(sys.argv[1])
    return make_socket(port)


class TestElegramServer(unittest.TestCase):
    def test_login(self):
        with connect() as sockobj:
            mesg2 = login_request(sockobj, "john", "12345678999")
            self.assertEqual(mesg2.response.status_response.result, StatusResponse.REJECTED)

            mesg1 = login_request(sockobj, "john", "12345678")
            self.assertEqual(mesg1.response.status_response.result, StatusResponse.ACCEPTED)

    def test_register(self):
        with connect() as sockobj:
            mesg1 = register_request(sockobj, "johny d", "johny_d@mail.ru", "12345678")
            self.assertEqual(mesg1.response.status_response.result, StatusResponse.ACCEPTED)

            mesg2 = register_request(sockobj, "john", "john_doe@mail.ru", "12345678")
            self.assertEqual(mesg2.response.status_response.result, StatusResponse.REJECTED)

    def test_get_contacts(self):
        with connect() as sockobj:
            mesg1 = login_request(sockobj, "john", "12345678")
            self.assertEqual(mesg1.response.status_response.result, StatusResponse.ACCEPTED)

            mesg2 = contacts_request(sockobj)
            self.assertEqual(len(mesg2.response.contacts_response.contacts), 2)
            self.assertEqual(mesg2.response.contacts_response.contacts[0].name, "jimm")
            self.assertEqual(mesg2.response.contacts_response.contacts[1].name, "jasck")

    def test_get_chats(self):
        with connect() as sockobj:
            mesg1 = login_request(sockobj, "john", "12345678")
            self.assertEqual(mesg1.response.status_response.result, StatusResponse.ACCEPTED)

            mesg2 = chats_request(sockobj)
            self.assertEqual(len(mesg2.response.chats_response.chats), 2)
            self.assertEqual(mesg2.response.chats_response.chats[0].chat_id, 1)
            self.assertEqual(mesg2.response.chats_response.chats[1].chat_id, 2)

    def test_get_messages(self):
        with connect() as sockobj:
            mesg1 = login_request(sockobj, "john", "12345678")
            self.assertEqual(mesg1.response.status_response.result, StatusResponse.ACCEPTED)

            chat_id = 1
            mesg2 = messages_request(sockobj, chat_id)
            self.assertEqual(len(mesg2.response.messages_response.messages), 4)
            self.assertEqual(mesg2.response.messages_response.messages[0].sender_id, 1)
            self.assertEqual(mesg2.response.messages_response.messages[0].chat_id, 1)
            self.assertEqual(mesg2.response.messages_response.messages[0].text, "Hello, jimm!")

    def test_send_message(self):
        with connect() as sockobj:
            mesg1 = login_request(sockobj, "john", "12345678")
            self.assertEqual(mesg1.response.status_response.result, StatusResponse.ACCEPTED)

            # user can write to this chat
            mesg2 = send_message_request(sockobj, 1, "Test message from python")
            self.assertEqual(mesg2.response.status_response.result, StatusResponse.ACCEPTED)

        with connect() as sockobj:
            mesg1 = login_request(sockobj, "jasck", "jasker37")
            self.assertEqual(mesg1.response.status_response.result, StatusResponse.ACCEPTED)

            # user can't write to this chat
            mesg2 = send_message_request(sockobj, 1, "U can't write to this chat")
            self.assertEqual(mesg2.response.status_response.result, StatusResponse.REJECTED)

            # chat not exist
            mesg3 = send_message_request(sockobj, 10, "chat not exist")
            self.assertEqual(mesg3.response.status_response.result, StatusResponse.REJECTED)


if __name__ == '__main__':
    unittest.main()
