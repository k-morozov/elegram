#!/usr/bin/python3

from __future__ import print_function

import struct
from socket import *

from messages_pb2 import *


def make_socket(port=4050):
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
    packed_len = struct.pack('>L', len(s))
    packed_message = packed_len + s
    sock.send(packed_message)


def socket_read_n(sock, n):
    """ Read exactly n bytes from the socket.
        Raise RuntimeError if the connection closed before n bytes were read.
    """
    buf = ''
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
    msg = Response()
    len_buf = socket_read_n(sock, 4)
    msg_len = struct.unpack('>L', len_buf)[0]
    msg_buf = socket_read_n(sock, msg_len)
    msg.ParseFromString(msg_buf)
    return msg


def send_login_request(sock: socket, login: str, passw: str):
    msg = WrappedMessage()
    msg.request = Request()
    msg.request.m2 = LoginRequest()
    msg.request.m2.name = login
    msg.request.m2.password = passw

    send_message(sock, msg)
    return get_response(sock)


# def send_set_value(sock, key, value):
#     rq = Request()
#     rq.type = Request.SET_VALUE
#     rq.request_set_value.key = key
#     rq.request_set_value.value = value
#     send_message(sock, rq)
#     return get_response(sock)
#
#
# def send_get_value(sock, key):
#     rq = Request()
#     rq.type = Request.GET_VALUE
#     rq.request_get_value.key = key
#     send_message(sock, rq)
#     return get_response(sock)
#
#
# def send_count_values(sock):
#     rq = Request()
#     rq.type = Request.COUNT_VALUES
#     send_message(sock, rq)
#     return get_response(sock)


if __name__ == '__main__':
    port = 9999
    if len(sys.argv) >= 2:
        port = int(sys.argv[1])

    sockobj = make_socket(port)

    print(send_login_request(sockobj, "john", "12345678"))
