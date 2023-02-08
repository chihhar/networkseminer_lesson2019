#!/usr/bin/env python
# -*- coding: utf-8-unix -*-

# client.py

# 実行方法: ./client.py
# （コンパイル不要）

import socket
import sys

host, port = "localhost", 13579
if (len(sys.argv) >= 2):
    host = sys.argv[1]
    if (len(sys.argv) >= 3):
        port = int(sys.argv[2])

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

try:
    sock.connect((host, port))
    sock.sendall("7\n")
    print("SEND: 7")
    recv_line = sock.recv(1024).rstrip("\n")
    print "RECV: " + recv_line

finally:
    sock.close()
