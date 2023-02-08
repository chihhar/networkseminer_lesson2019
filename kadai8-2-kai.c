#include<stdio.h>
#include<stdlib.h>
#include<netdb.h>
#include<pcap/pcap.h>
#include<ctype.h>

char errbuf[PCAP_ERRBUF_SIZE];

void print_ipv4(u_char *packet);
void print_tcp(u_char *packet, u_short len);
void print_unknown(u_char *packet);


int main(int argc,char *argv[])
{
  int cnt = 0;
  int max_cnt;
  pcap_if_t *alldevsp;
  char *device;
  pcap_t *p;
  u_char * packet;
  struct pcap_pkthdr h;
  bpf_u_int32 net;
  bpf_u_int32 mask;
  struct bpf_program f;
  
  int n=0;
  unsigned short e_type;
  
  switch(argc){
  case 1:
    max_cnt = 20;
    break;
  case 2:
    max_cnt = strtol(argv[1],NULL,10);
    if(max_cnt > 0)
      break;
  default:
    printf("Usage: %s [max_cnt]\n",argv[0]);
    exit(1);
    break;
  }

  if(pcap_findalldevs(&alldevsp, errbuf) == -1){
    fprintf(stderr, "%s\n",errbuf);
    exit(1);
  }
  if(alldevsp == NULL){
    fprintf(stderr,"%s\n", errbuf);
    exit(1);
  }
  device = alldevsp->name;
 
  if(pcap_lookupnet(device, &net,&mask, errbuf) == -1){
    fprintf(stderr, "%s\n", errbuf);
    exit(2);
  }
  
  p=pcap_open_live(device,1518,1,500,errbuf);
  if(p == NULL){
    fprintf(stderr,"%s\n",errbuf);
    exit(2);
  }

  if(pcap_compile(p,&f,"tcp",1,mask)==-1){
    fprintf(stderr, "%s\n",pcap_geterr(p));
    exit(4);
  }

  if(pcap_setfilter(p, &f) == -1){
    fprintf(stderr, "%s\n",pcap_geterr(p));
    exit(5);
  }
  
  setbuf(stdout,NULL);
  
  while(cnt < max_cnt){

    packet = (u_char *)pcap_next(p,&h);

		if(packet != NULL){
			cnt++;
			//printf("[%02d]\n",cnt);
		//print_ethernet(packet);
			e_type = (unsigned short)packet[12] << 8;
			e_type += (unsigned short)packet[13];
			switch(e_type){
				case 0x0800:
					print_ipv4(&packet[14]);
					break;
				default:
					print_unknown(&packet[14]);
					break;
			}
		}
  }
  printf("\n%d packets have been captured.\n", cnt);

  pcap_close(p);

  return 0;
}

void print_ipv4(u_char *packet)
{
	int n;
	n = 0;
	int type;
	int protocol;
	int ihl;
	int dgl;
	int tcp_s;
	int tcp_l;
	int data_l;
	ihl = packet[0] - ((packet[0]>>4)<<4);
	dgl = (packet[2]<<8) + packet[3];
	type = (packet[4]<<8) + packet[5];
	protocol = packet[9];
    for(n=12;n<16;n++){
		printf(((n<15) ? "%d." : "%d:"),packet[n]);
	}
	tcp_s = 4*ihl;
    printf("%d",((packet[tcp_s]<<8)+packet[tcp_s + 1])); 
    printf(" -> ");
    for(n=16;n<20;n++){
		printf(((n<19) ? "%d." : "%d:"),packet[n]);
    }
    printf("%d ",((packet[tcp_s + 2]<<8)+packet[tcp_s + 3]));
    int value;
    value = (packet[tcp_s+13] - (1<<7)) - (1<<6);
    for(int j=5;j>=0;j--){
		if(value & (1<<j)){
			switch(j){
				case 5:
				printf("U");
				break;
				case 4:
				printf("A");
				break;
				case 3:
				printf("P");
				break;
				case 2:
				printf("R");
				break;
				case 1:
				printf("S");
				break;
				case 0:
				printf("F");
				break;
				default:;
			}
		}
	}
	switch(protocol){
      case 6:
	  tcp_l = packet[(tcp_s)+12]>>4;
	  data_l = dgl - tcp_s - 4*tcp_l;
	  printf("\n");
	  if(data_l > 0){
		  print_tcp(&packet[4*ihl], dgl - 4*ihl);
	  }
	  break;
	  default:
	  print_unknown(&packet[4*ihl]);
	  break;
	}
	return;
}

void print_tcp(u_char *packet, u_short len)
{
	char *protocol;
    int c;
	int port;
	int tcp_l; 
	int value;
	int ck=0;
	int data_l;
  
	tcp_l = (packet[12]>>4); 
	port = tcp_l;
	value = (packet[18] << 8) + packet[19];
	//printf("Data:\n");
	int ck_loop=0;
	data_l = len - 4*tcp_l;
	for(int num=4*tcp_l; num< 4*tcp_l + data_l;num++){
		c = packet[num];
		if(isprint(c)){
			printf(" %c",c);
			ck_loop=0;
		}else{
			if(ck_loop == 0){
				printf("\n");
				}
			ck_loop++;
		}
	}
	printf("\n");
	return;
}
  
void print_unknown(u_char *packet)
{
  int n;
  int type;
  printf("------------------------------------\n");

  printf("Data: ");
  for(n=0;n < 8;n++){
    printf(((n < 7) ? "%02x " : "%02x...\n"), packet[n]);
  }

  return;
}
