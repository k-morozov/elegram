#!/usr/bin/python3

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
    request.login_request.MergeFrom(register_req)

    msg = WrappedMessage()
    msg.request.MergeFrom(request)

    send_message(sock, msg)
    return get_response(sock)


class TestElegramServer(unittest.TestCase):
    def setUp(self):
        self.port = 8000
        if len(sys.argv) >= 2:
            self.port = int(sys.argv[1])
        self.sockobj = make_socket(self.port)

    def test_login(self):
        mesg1 = login_request(self.sockobj, "john", "12345678")
        self.assertEqual(mesg1.response.status_response.result, StatusResponse.ACCEPTED)

        mesg2 = login_request(self.sockobj, "john", "12345678999")
        self.assertEqual(mesg2.response.status_response.result, StatusResponse.REJECTED)

    def test_register(self):
        pass
        # resp1 = register_request(self.sockobj, "johny", "johny@mail.ru", "12345678")
        # self.assertEqual(resp1.status_response.result, StatusResponse.ACCEPTED)
        # resp1 = register_request(self.sockobj, "john", "john_doe@mail.ru", "12345678")
        # self.assertEqual(resp1.status_response.result, StatusResponse.REJECTED)

    def tearDown(self):
        self.sockobj.close()


if __name__ == '__main__':
    unittest.main()
