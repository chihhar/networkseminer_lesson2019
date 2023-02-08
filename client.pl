#!/usr/bin/env perl

# client.pl

# 実行方法: ./client.pl
# （コンパイル不要）

use IO::Socket;

$host = 'localhost';
$port = 13579;
if ($#ARGV >= 0) {
    $host = $ARGV[0];
}
if ($#ARGV >= 1) {
    $port = $ARGV[1] + 0;
}

$sock = new IO::Socket::INET(PeerAddr => $host,
                             PeerPort => $port,
                             Proto => 'tcp');

die "IO::Socket : $!" unless $sock;

print $sock "5\n";
print "SEND: 5\n";

$recv_line = <$sock>;
chomp($recv_line);
print "RECV: $recv_line\n";

close($sock);
