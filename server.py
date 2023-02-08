#!/usr/bin/env python
# -*- coding: utf-8-unix -*-

# server.py

# 実行方法: ./server.py
# （コンパイル不要）

import sys
import SocketServer

class Handler(SocketServer.StreamRequestHandler):
    def handle(self):
        recv_line = self.rfile.readline().rstrip("\n")
        print("RECV: " + recv_line)
        if len(recv_line) == 0:
            send_line = ""
        else:
            recv_num = int(recv_line)
            send_line = str(recv_num) * recv_num
        self.wfile.write(send_line + "\n")
        print("SEND: " + send_line)

port = 13579
if (len(sys.argv) >= 2):
    port = int(sys.argv[1])

SocketServer.TCPServer.allow_reuse_address = True
server = SocketServer.TCPServer(('127.0.0.1', port), Handler)
server.handle_request()
