#include <time.h>
#define RTP_HEADER_LEN 12 // bytes

int init_connection(char host[], uint16_t port)
{
  struct sockaddr_in connection_address;
  int sockfd;

  if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    puts("SOCKET ERROR");
    return -1;
  }

  connection_address.sin_family = AF_INET;
  connection_address.sin_addr.s_addr = inet_addr(host);
  connection_address.sin_port = htons(port);

  if(connect(sockfd, (struct sockaddr *)&connection_address, sizeof(connection_address)) < 0) {
    printf("CONNECTION ERROR!\n");
    return 0;
  }

  return sockfd;
}

typedef struct {
  unsigned short active:1;
  int pid;
  int sockfd;
  uint16_t client_port;
  uint16_t server_port;
  char* session;
} bridge;


typedef struct {
  unsigned char padding;
  unsigned char csrc_count;
  unsigned char marker;
  unsigned short int sequence_number;
  unsigned int timestamp;
} RTP_HEADER;

// RTP
RTP_HEADER* ParseRTPHeader(char* packet)
{

  RTP_HEADER header;

  // Version (supported specification is 2)
  if( !(packet[0]&0x80) ) return -1;

  // Padding bit
  header.padding = (packet[0]>>5) - 0b100;

  // CSRC count (CC)
  header.csrc_count = (packet[0]<<4);
  csrc_count=csrc_count>>4;

  // Marker bit
  header.marker = packet[1]>>7;

  // Payload type: ignore for now

  header.sequence_number = packet[2];
  sequence_number = sequence_number<<8;
  sequence_number += packet[3];

  header.timestamp = packet[4];
  timestamp = (timestamp<<24);
  timestamp += (0x00+packet[5])<<16;
  timestamp += (0x00+packet[6])<<8;
  timestamp += packet[7];

  // SSRC and CSRC list is not implemented

  return &header;
}


// Currently only support Fragmented Units to be processed
// process: ( FU indicator  and   FU header )
int ParseH264Header(char* packet)
{
  // FU indicator
  unsigned char forbidden_zero_bit = packet[0]>>7;
  if(forbidden_zero_bit) return -1; // drop packet

  unsigned char type = packet[0]<<3;
  type = type>>3;

  if( type == 28 ); // FU-A
  else if( type == 29 ); // FU-B | not supported

  // Fu header
  unsigned char boundary_bit = 0; // 1 when start indicator, 2 when end indicator is set
  if( (packet[1]&0x80) ) boundary_bit = 1;
  else if( (packet[1]&0x40) ) boundary_bit = 2;

  return type
}

bridge* camera_feed_bridge()
{
  struct sockaddr_in stream_connection, camera_client;
  int sockfd;

  if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    puts("SOCKET ERROR");
    // return -1;
    exit(1);
  }

  stream_connection.sin_family = AF_INET;
  stream_connection.sin_addr.s_addr = inet_addr("0.0.0.0");

  bridge *camera_bridge = (bridge*) malloc(sizeof(bridge));
  #define port camera_bridge->client_port
  camera_bridge->sockfd = sockfd;

  time_t t;
  int co;

  // Choose random port is user didn't provide one
  while(1) {
    srand((unsigned)time(&t));

    port = 4000+(rand()%5000);
    stream_connection.sin_port = htons(port);

    co = bind(sockfd, (struct sockaddr *)&stream_connection, sizeof(stream_connection));
    if(co < 0) {
      printf("Occupied port number: %d\n", port);
      exit(1);
    }
    else {
      break;
    }
  }

  printf("PORT: %d\n", port);
  #undef port

  // Should call video player when traffic incomes
  camera_bridge->pid = 0;
  camera_bridge->active = 1;

  if(fork() == 0) {

    int PC = 20;
    char* stream_data = (char*)malloc(6557);

    while(PC) {
      memset(stream_data, 6557, 0);
      int recv_len;

      // recv_len = recvfrom(sockfd, stream_data, 6556, 0, (struct sockaddr*)&camera_client, (socklen_t*)sizeof(camera_client));
      recv_len = recvfrom(sockfd, stream_data, 6556, 0, NULL, NULL);
      if(recv_len < 8) puts("RTP header is missing!");
      ParseRTPHeader(stream_data);
      ParseH264Header(stream_data+RTP_HEADER_LEN);

      PC--;
    }

    // DEBUG:
    // printf("%d\n", recv_len);
    // for(int i=0;i<recv_len;i++) {
      // printf("%#.8x\n", stream_data[i]);
    // }
    // printf("%p %#.8X %d %d\n", stream_data[1], stream_data[1], recv_len, sockfd);

    free(stream_data);
    exit(0);
  }


  return camera_bridge;
}
