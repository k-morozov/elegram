#!/usr/bin/python3

from __future__ import print_function

from socket import *

from messages_pb2 import *


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
    len_buf = socket_read_n(sock, 9)
    len_prefix.ParseFromString(len_buf)
    print("Response prefix length", len_prefix.length)

    msg_buf = socket_read_n(sock, len_prefix.length)

    msg = Response()
    msg.ParseFromString(msg_buf)

    return msg


def send_login_request(sock, login, passw):
    login_req = LoginRequest()
    login_req.name = login
    login_req.password = passw

    request = Request()
    request.login_request.MergeFrom(login_req)

    msg = WrappedMessage()
    msg.request.MergeFrom(request)

    send_message(sock, msg)
    return get_response(sock)


if __name__ == '__main__':
    port = 8000
    if len(sys.argv) >= 2:
        port = int(sys.argv[1])

    sockobj = make_socket(port)

    resp = send_login_request(sockobj, "john", "12345678")
    if resp.status_response.result == StatusResponse.ACCEPTED:
        print("ACCEPTED")
    elif resp.status_response.result == StatusResponse.REJECTED:
        print("REJECTED")
    else:
        raise Exception()
