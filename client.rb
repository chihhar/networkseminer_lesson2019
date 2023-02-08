#!/usr/bin/env ruby

# client.rb

# 実行方法: ./client.rb
# （コンパイル不要）

require "socket"

case ARGV.first
when /:/
  host = $`
  port = $'.to_i
when String
  host = ARGV.first
  port = ARGV.length >= 2 ? ARGV[1].to_i : 13579
else
  host = "localhost"
  port = 13579
end

socket = TCPSocket.open(host, port)
socket.write "9\n"
puts "SEND: 9"
recv_line = socket.gets.chomp
puts "RECV: #{recv_line}"
socket.close
