#include<stdio.h>
#include<stdlib.h>
#include<netdb.h>
#include<pcap/pcap.h>
#include<ctype.h>

char errbuf[PCAP_ERRBUF_SIZE];

void print_ethernet(u_char *packet);

void print_arp(u_char *packet);
void print_ipv4(u_char *packet);
void print_icmp(u_char *packet);
void print_tcp(u_char *packet, u_short len);
void print_udp(u_char *packet);
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

  p=pcap_open_live(device,96,1,500,errbuf);
  if(p == NULL){
    fprintf(stderr,"%s\n",errbuf);
    exit(2);
  }

  setbuf(stdout,NULL);
  while(cnt < max_cnt){

    packet = (u_char *)pcap_next(p,&h);

    if(packet != NULL){
      cnt++;
      printf("[%02d]\n",cnt);
      print_ethernet(packet);
    }
  }
  printf("\n%d packets have been captured.\n", cnt);

  pcap_close(p);

  return 0;
}
void print_ethernet(u_char *packet)
{
  int n=0;
  unsigned short e_type;
  printf("==================================\n");
  printf("Dst. MAC Addr.: ");
  for(n=0; n<6;n++){
    printf(((n<5) ? "%02x:" : "%02x\n"),packet[n]);
  }
  printf("Src. MAC Addr.: ");
  for(n=6;n<12;n++){
    printf(((n<11) ? "%02x:" : "%02x\n"),packet[n]);
  }
  printf("Type: ");

  e_type = (unsigned short)packet[12] << 8;
  e_type += (unsigned short)packet[13];

  switch(e_type){
  case 0x0806:
    printf("ARP\n");
    print_arp(&packet[14]);
    break;
  case 0x0800:
    printf("IPv4\n");
    print_ipv4(&packet[14]);
    break;
  default:
    printf("unknown type\n");
    print_unknown(&packet[14]);
    break;
  }
  printf("===================================\n");
  return;
}
void print_arp(u_char *packet)
{
  int n;
  n=0;
  int type;
  printf("------------------------------------\n");
  while(1){
    if(n==0){
      printf("Hardware Addr. Space: ");
      type = (packet[n]<<8) + packet[n+1];
    if(type==0x0001){
      printf("Ethernet\n");
    }else{
      printf("unknown\n");
    }
  }
  if(n==2){
    printf("Protocol Addr.: ");
    type = (packet[n]<<8)+packet[n+1];
    if(type==0x0800){
      printf("IPv4\n");
    }else{
      printf("unknown\n");
    }
  }
  if(n==4){
    printf("hln: ");
    printf("%2x\n",packet[n]);
  }
  if(n==5){
    printf("pln: ");
    printf("%2x\n",packet[n]);
  }
  if(n==6){
    type = (packet[n]<<8) + packet[n+1];
    printf("opcode: ");
    switch(type){
    case 0x0001:
      printf("REQUEST\n");break;
    case 0x0002:
      printf("REPLY\n");
      break;
    default:printf("UNKNOWN\n");break;
    }
  }
  if(n==8){
    printf("Hardware Addr. of Sender: ");
    for(n=8;n<14;n++){
      printf(((n<13) ? "%02x:" : "%02x\n"), packet[n]);
    }
  }
  if(n==14){
    printf("Protocol Addr. of Sender: ");
    for(n=14;n<18;n++){
      printf(((n<17) ? "%d." : "%d\n"), packet[n]);
    }
  }
  if(n==18){
    printf("Hardware Addr. of Target: ");
    for(n=18;n<24;n++){
      printf(((n<23) ? "%02x:" : "%02x\n"), packet[n]);
    }
  }
  if(n==24){
    printf("Protocol Addr. of Target: ");
    for(n=24;n<28;n++){
      printf(((n<27) ? "%d." : "%d\n"), packet[n]);
    }
  }
  if(n>=28){
    break;
  }
  n++;
  }
  return;
}
void print_ipv4(u_char *packet)
{
  int n;
  n = 0;
  int type;
  int protocol;
  int ihl;
  int dgl;
  printf("------------------------------------\n");
  while(1){
    if(n==0){
      printf("Version: ");
      printf("%d\n",(packet[0]>>4));
      printf("IHL: ");
      ihl = packet[0] - ((packet[0]>>4)<<4);
      printf("%d\n",ihl);
    }
    if(n==1){
      printf("ToS: ");
      printf("%d\n",packet[n]);
    }
    if(n==2){
      printf("Total Length: ");
      dgl = (packet[n]<<8) + packet[n+1];
      printf("%d\n",dgl);
    }
    if(n==4){
      printf("Identification: ");
      type = (packet[n]<<8) + packet[n+1];
      printf("%d\n",type);
    }
    if(n==6){
      printf("Frags: ");
      printf("%d\n",packet[n]>>5);
      printf("Fragment Offset: ");
      printf("%d\n",((packet[n] - ((packet[n]>>5)<<5))<<8)+packet[n+1]);
    }
    if(n==8){
      printf("TTL: ");
      printf("%x\n",packet[n]);
    }
    if(n==9){
      protocol = packet[9];
      printf("Protocol: ");
      switch(packet[n]){
      case 1:
	printf("ICMP\n");
	break;
      case 6:
	printf("TCP\n");
	break;
      case 17:
	printf("UDP\n");
	break;
      default:
	printf("unknown\n");
      }
    }
    if(n==10){
      printf("Checksum: ");
      printf("%d\n",(packet[n]<<8) + packet[n+1]);
    }
    if(n==12){
      printf("Src. IP Addr.: ");
      for(n=12;n<16;n++){
	printf(((n<15) ? "%d." : "%d\n"),packet[n]);
      }
    }
    if(n==16){
      printf("Dst. IP Addr.: ");
      for(n=16;n<20;n++){
	printf(((n<19) ? "%d." : "%d\n"),packet[n]);
      }
    }
    if(n==20){
      switch(protocol){
      case 1:
	print_icmp(&packet[4*ihl]);
	break;
      case 6:
	print_tcp(&packet[4*ihl], dgl - 4*ihl);
	break;
      case 17:
	print_udp(&packet[4*ihl]);
	break;
      default:
	print_unknown(&packet[4*ihl]);
	break;
      }
      
      break;
    }
    n++;
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
  printf("------------------------------------\n");
  printf("Source Port: ");
  port = (packet[0]<<8) + packet[1];
  printf("%d\n",port);
  port = (packet[2]<<8) + packet[3];
  printf("Destination Port: %d\n",port);
  value =(packet[4]<<24)+(packet[5]<<16)+(packet[6]<<8)+packet[7];
  printf("Sequence Number: %x\n",value);
  value =(packet[8]<<24)+(packet[9]<<16)+(packet[10]<<8) + packet[11];
  printf("Acknowledgement Number: %x\n", value);
  tcp_l = (packet[12]>>4); 
  port = tcp_l;
  printf("Data Offset: %d\n",tcp_l);
  value = (packet[13] - (1<<7)) - (1<<6);
  printf("Control Bit: ");
  for(int i = 5;i>=0;i--){
    if(value & (1 << i)){
      switch(i){
	case 5:
	  printf("URG");
	  ck++;
	  break;
	case 4:
	  if(ck>0){
	    printf(",");
	  }
	  printf("ACK");
	  ck++;
	  break;
	case 3:
	  if(ck>0){
	    printf(",");
	  }
	  printf("PSH");
	  ck++;
	  break;
	case 2:
	  if(ck>0){
	    printf(",");
	  }
	  printf("RST");
	  ck++;
	  break;
	case 1:
	  if(ck>0){
	    printf(",");
	  }
	  printf("SYN");
	  ck++;
	  break;
	case 0:
	  if(ck>0){
	    printf(",");
	  }
	  printf("FIN");
	  break;
      default:;
      }
    }
  }
  printf("\n");
  value = (packet[14]<<8) + packet[15];
  printf("Window: %d\n",value);
  value = (packet[16] << 8) + packet[17];
  printf("Checksum: %d\n",value);
  value = (packet[18] << 8) + packet[19];
  printf("Urgent pointer: %d\n",value);
  printf("------------------------------------\n");

  printf("Data:\n");
  data_l = len - 4*tcp_l;
  if(data_l<16){
    for(int num=4*tcp_l; num< 4*tcp_l + data_l;num++){
      printf(" %02x",packet[num]);
    }
    printf("\n");
    for(int num=4*tcp_l; num< 4*tcp_l + data_l;num++){
      c = packet[num];
      if(isprint(c)){
	printf(" %c",c);
      }else{
	printf(" .");
      }
    }
    printf("\n");
  }else{
    for(int num=4*tcp_l;num < (tcp_l*4) +16; num++){
      printf(" %02x",packet[num]);
    }
    printf("\n");
    for(int num=4*tcp_l;num < tcp_l*4 + 16;num++){
      c = packet[num];
      if(isprint(c)){
	printf(" %c",c);
      }else{
	;printf(" .");
      }
    }
    printf("\n");
  }

  
  return;
}
void print_icmp(u_char *packet)
{
  int type;
  int check;
  int id;
  int sn;
  printf("------------------------------------\n");
  printf("Type");
  type = packet[0];
  if(type == 0){
    printf(" Echo Reply\n");
  }else if(type == 8){
    printf(" Echo\n");
  }else{
    printf(" Unknown\n");
  }
  printf("Code: %d\n",packet[1]);
  
  printf("Checksum:");
  check = (packet[2]<<8)+packet[3];
  printf(" %d\n",check);
  id = (packet[4]<<8) + packet[5];
  sn = (packet[6]<<8) + packet[7];
  switch(type){
  case 0:
    printf("Identifier");
    printf(" %d\n",id);
    printf("Sequence Number: %d\n",sn);
    break;
  case 8:
    printf("Identifier %d\n",id);
    printf("Sequence Number: %d\n",sn);
    break;
  default:
    printf("UNKNOWN\n");
  }
  printf("=======================================\n");
  return;
}

void print_udp(u_char *packet)
{
  int value;
  int len;
  int c;
  printf("------------------------------------\n");
  value = (packet[0]<<8)+packet[1];
  printf("Source Port: %d\n",value);
  value = (packet[2]<<8)+packet[3];
  printf("Destination Port: %d\n",value);
  len = (packet[4]<<8)+packet[5];
  printf("Length: %d\n",len);
  value = (packet[6]<<8)+packet[7];
  printf("Checksum: %d\n",value);
  printf("------------------------------------\n");
  len = len-8;
  if(len < 16){
    for(int num=0;num<len;num++){
      printf(" %02x",packet[8+num]);
    }
    printf("\n");
    for(int num=0;num<len;num++){
      c = packet[8+num];
      if(isprint(c)){
	printf(" %c",c);
      }else{
	printf(" .");
      }
    }
      printf("\n");
  }else{
    for(int num = 0;num<16;num++){
      printf(" %02x",packet[8+num]);
    }
    printf("\n");
    for(int num = 0;num<16;num++){
      c = packet[8+num];
      if(isprint(c)){
	printf(" %c",c);
      }else{
	printf(" .");
      }
    }
    printf("\n");
    }
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
