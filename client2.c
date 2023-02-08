/* client2.c */

/* コンパイル方法: cc client2.c -o client2 */
/* 実行方法: ./client2                     */

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define DEFAULT_PORT "13579"

/* ホスト名とポート番号からなる構造体
   （関数呼び出しで2つの文字列を戻り値とするため） */
struct hostport {
  char *hostname;
  char *port;
};

struct hostport get_hostport(int argc, char *argv[]);
int connect_to_server(char *hostname, char *port);
void send_and_receive(int s);
void print_error_and_exit(char *message);

/* コマンドライン引数から接続先情報を獲得する */
struct hostport get_hostport(int argc, char *argv[])
{
  struct hostport hp;
  char *colon;

  switch (argc) {
  case 1:
    hp.hostname = "localhost";
    hp.port = DEFAULT_PORT;
    break;
  case 2:
    hp.hostname = argv[1];
    if ((colon = strchr(hp.hostname, ':')) != NULL) {
      hp.port = colon + 1;
      *colon = '\0';
    } else {
      hp.port = DEFAULT_PORT;
    }
    break;
  default:
    hp.hostname = argv[1];
    hp.port = argv[2];
    break;
  }

  return hp;
}

/* サーバに接続する */
int connect_to_server(char *hostname, char *port)
{
  struct addrinfo hints;
  struct addrinfo *result;
  struct addrinfo *rp;
  int s;

  /* ソケットアドレス構造体を生成する */
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC;
  // hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  if (getaddrinfo(hostname, port, &hints, &result) != 0) {
    print_error_and_exit("Fail: getaddrinfo\n");
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
    print_error_and_exit("Fail: connect\n");
  }

  freeaddrinfo(result);

  return s;
}

/* 1行送信してから，受信する */
void send_and_receive(int s)
{
  int c;

  write(s, "3", 1);
  write(s, "\n", 1);
  printf("SEND: 3\n");

  printf("RECV: ");
  while (c != '\n' && read(s, &c, 1) > 0) {
    putchar(c);
  }
}

/* エラーメッセージを出力して終了する */
void print_error_and_exit(char *message)
{
  fputs(message, stderr);
  exit(1);
}

int main(int argc, char *argv[])
{
  struct hostport hp;
  int s;

  hp = get_hostport(argc, argv);
  s = connect_to_server(hp.hostname, hp.port);
  send_and_receive(s);
  close(s);

  return 0;
}
