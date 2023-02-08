#!/usr/bin/env ruby

# server.rb

# 実行方法: ./server.rb
# （コンパイル不要）

require "socket"

port = (ARGV.shift || 13579).to_i

TCPServer.open("127.0.0.1", port) do |server|
  socket = server.accept
  recv_line = socket.gets.chomp
  puts "RECV: #{recv_line}"
  if recv_line.empty?
    socket.write "\n"
    puts "SEND: "
  else
    num = recv_line.to_i
    send_line = ""
    while num > 1
      send_line += num.to_s
      if num % 2 == 1
        num = num * 3 + 1
      else
        num /= 2
      end
    end
    send_line += num.to_s
    socket.write "#{send_line}\n"
    puts "SEND: #{send_line}"
  end
end
