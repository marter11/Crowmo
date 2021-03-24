#include<stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include<string.h>
#include<unistd.h>

#define HOST "192.168.0.113"
#define PORT 554

void init_connection(char* host, uint16_t port)
{
  struct sockaddr_in addr;
  int udp,sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = inet_addr("0.0.0.0");
  addr.sin_port = htons(5000);

  udp = bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));

  recvfrom(sockfd, data, 170, 0, NULL, NULL);
  printf("%s\n", data);

  exit(0);
}

int main()
{
  // pid_t fork_id;
  // char* data = (char*)malloc(170);
  // memset(data, 170, 0);
  //
  // if(fork() == 0) init_connection(HOST, PORT, data);
  // else printf("%d\n", fork_id);

  

  return 0;
}
