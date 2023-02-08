/* client1.c */

/* コンパイル方法: cc client1.c -o client1 */
/* 実行方法: ./client1                     */

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define DEFAULT_PORT "13579"

int main(int argc, char *argv[])
{
  char *hostname, *port;
  char *colon;
  struct addrinfo hints;
  struct addrinfo *result;
  struct addrinfo *rp;
  int s;
  char c;

  /* コマンドライン引数から接続先情報を獲得する */
  switch (argc) {
  case 1:
    hostname = "localhost";
    port = DEFAULT_PORT;
    break;
  case 2:
    hostname = argv[1];
    if ((colon = strchr(hostname, ':')) != NULL) {
      port = colon + 1;
      *colon = '\0';
    } else {
      port = DEFAULT_PORT;
    }
    break;
  default:
    hostname = argv[1];
    port = argv[2];
    break;
  }

  /* ソケットアドレス構造体を生成する */
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC;
  // hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  if (getaddrinfo(hostname, port, &hints, &result) != 0) {
    fprintf(stderr, "Fail: getaddrinfo\n");
    exit(1);
  }

  /* ソケットを作成し，接続する */
  for (rp = result; rp != NULL; rp = rp->ai_next) {
    s = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
    if (s == -1) {
      continue;
    }
    if (connect(s, rp->ai_addr, rp->ai_addrlen) != -1) {
      break;
    }
    close(s);
  }
  if (rp == NULL) {
    fprintf(stderr, "Fail: connect\n");
    exit(1);
  }

  freeaddrinfo(result);

  /* この時点でのsの値が，サーバと通信するための
     ソケットディスクリプタになっている */

  /* 1行送信する */
  write(s, "3", 1);
  write(s, "\n", 1);
  printf("SEND: 3\n");

  /* 1行受信する */
  printf("RECV: ");
  while (c != '\n' && read(s, &c, 1) > 0) {
    putchar(c);
  }

  /* ソケットを閉じて終了する */
  close(s);

  return 0;
}
