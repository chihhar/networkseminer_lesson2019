/* server1.c */

/* コンパイル方法: cc server1.c -o server1 */
/* 実行方法: ./server1                     */

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netdb.h>
#include <signal.h>

#define DEFAULT_PORT "13579"

/* acceptに失敗したときの処理 */
void fail_accept(int sig)
{
  fprintf(stderr, "Failed: accept\n");
  exit(1);
}

/* 1行受信に失敗したときの処理 */
void fail_receive(int sig)
{
  fprintf(stderr, "Failed: receive\n");
  exit(1);
}

int main(int argc, char *argv[])
{
  char *port;
  struct addrinfo hints;
  struct addrinfo *result;
  struct addrinfo *rp;
  int s1, s2;
  int opt = 1;
  struct sockaddr_in sin_client;
  socklen_t sin_client_length = sizeof(sin_client);
  char c;
  char message[3];
  int recv_num;
  int send_num;

  /* コマンドライン引数からポート番号を獲得する */
  if (argc >= 2) {
    port = argv[1];
  } else {
    port = DEFAULT_PORT;
  }

  /* ソケットアドレス構造体を生成する */
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;
  if (getaddrinfo(NULL, port, &hints, &result) != 0) {
    fprintf(stderr, "Failed: getaddrinfo\n");
    exit(1);
  }

  /* ソケットアドレス構造体を用いて，ソケットに名前をつける */
  for (rp = result; rp != NULL; rp = rp->ai_next) {
    s1 = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
    if (s1 == -1) {
      continue;
    }
    if (setsockopt(s1, SOL_SOCKET, SO_REUSEADDR, (void *)&opt, sizeof(int)) == 0
        && bind(s1, rp->ai_addr, rp->ai_addrlen) == 0) {
      break;
    }
    close(s1);
  }
  if (rp == NULL) {
    fprintf(stderr, "Failed: bind\n");
    exit(1);
  }

  freeaddrinfo(result);

  /* この時点でのs1の値は，クライアントからの接続を受け付けるためのソケット
     ディスクリプタであり，クライアントと通信するためのものではない． */

  /* 接続を待つ */
  if (listen(s1, 1) < 0) {
    fprintf(stderr, "Failed: listen\n");
    exit(1);
  }

  /* 30秒以内にクライアントからの接続がなければ，終了する */
  alarm(30);
  signal(SIGALRM, fail_accept);

  /* クライアントと通信するためのソケットを獲得する */
  if ((s2 = accept(s1, (struct sockaddr *)&sin_client, &sin_client_length)) < 0) {
    fail_accept(0);
  }

  /* 5秒以内にクライアントからメッセージを受け取らなければ，終了する */
  alarm(5);
  signal(SIGALRM, fail_receive);

  /* この時点でのs2の値が，クライアントと通信するための
     ソケットディスクリプタになっている */

  /* 1行受信する */
  recv_num = 0;
  printf("RECV: ");
  while (c != '\n' && read(s2, &c, 1) > 0) {
    printf("%c", c);
    if (isdigit(c)) {
      recv_num = c - '0';
    }
  }

  /* 1行送信する */
  send_num = recv_num + 1;
  sprintf(message, "%d", send_num);
  printf("SEND: %s\n", message);
  write(s2, message, strlen(message));
  write(s2, "\n", 1);

  /* アラームを止める */
  alarm(0);
  signal(SIGALRM, SIG_DFL);

  /* ソケットを閉じて終了する */
  close(s2);
  close(s1);

  return 0;
}
