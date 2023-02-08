#!/usr/bin/env perl

# server.pl

# 実行方法: ./server.pl
# （コンパイル不要）

use IO::Socket;

$port = 13579;
if ($#ARGV >= 0) {
    $port = $ARGV[0] + 0;
}

$sock0 = new IO::Socket::INET(Listen => 1,
                              LocalAddr => 'localhost',
                              LocalPort => $port,
                              Proto => 'tcp',
                              Reuse => 1);

die "IO::Socket : $!" unless $sock0;

$sock = $sock0->accept();

$recv_line = <$sock>;
chomp($recv_line);
print "RECV: $recv_line\n";

if ($recv_line eq '')
{
    $send_line = "\n";
} else {
    $recv_num = $recv_line + 0;
    $send_num = $recv_num + $recv_num;
    $send_line = $send_num + "";
}

print $sock "$send_line\n";
print "SEND: $send_line\n";

close($sock);
close($sock0);

exit;
