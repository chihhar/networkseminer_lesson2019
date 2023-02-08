/* server2.c */

/* コンパイル方法: cc server2.c -o server2 */
/* 実行方法: ./server2                     */

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


void fail_accept(int sig);
void fail_receive(int sig);
char* get_port(int argc,char *argv[]);
int connect_to_client(char *port);
void listen_connect(int s1);
int get_socket(int s2);
void receive_and_send;
void print_error_and_exit;

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

/* コマンドライン引数からポート番号を獲得する */
char* get_port(int argc,char *argv[])
{
	if (argc >= 2){
		return = argv[1];
	} else {
		return = DEFAULT_PORT;
  }
}

int connect_to_client(char *port)
{
	struct addrinfo hints;
	struct addrinfo *result;
	struct addrinfo *rp;
	int opt = 1;
	
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
  
  return s1;
}


int main(int argc, char *argv[])
{
	int s1;
	char *port;
	
	port = get_port(argc,argv);
	connect_to_client(port);
  return 0;
}
