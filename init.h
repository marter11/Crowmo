#include <time.h>
#define RTP_HEADER_LEN 12 // bytes

// Error codes
#define VALID_HEADER 0
#define INVALID_HEADER 1

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
int ParseRTPHeader(RTP_HEADER* header, char* packet)
{

  // Version (supported specification is 2)
  if( !(packet[0]&0x80) ) return INVALID_HEADER;
  else {

    // Padding bit
    header->padding = (packet[0]>>5) - 0b100;

    // CSRC count (CC)
    header->csrc_count = (packet[0]<<4);
    header->csrc_count=header->csrc_count>>4;

    // Marker bit
    header->marker = packet[1]>>7;

    // Payload type: ignore for now

    header->sequence_number = packet[2];
    header->sequence_number = header->sequence_number<<8;
    header->sequence_number += packet[3];

    header->timestamp = packet[4];
    header->timestamp = (header->timestamp<<24);
    header->timestamp += (0x00+packet[5])<<16;
    header->timestamp += (0x00+packet[6])<<8;
    header->timestamp += packet[7];

    // SSRC and CSRC list is not implemented
  }

  return VALID_HEADER;
}


// Currently only support Fragmented Units to be processed
// process: ( FU indicator  and   FU header )
typedef struct {
  unsigned char type;
  unsigned char boundary_bit;
} H264Header;

int ParseH264Header(H264Header* header, char* packet)
{
  // FU indicator
  unsigned char forbidden_zero_bit = packet[0]>>7;
  if(forbidden_zero_bit) return INVALID_HEADER; // drop packet

  header->type = packet[0]<<3;
  header->type = header->type>>3;

  if(header->type == 7 || header->type == 8)
  {
    header->boundary_bit = 0;
  }

  else if( header->type == 28 ) // FU-A
  {
    // Fu header
    header->boundary_bit = 0; // 1 when start indicator, 2 when end indicator is set
    if( (packet[1]&0x80) ) header->boundary_bit = 1;
    else if( (packet[1]&0x40) ) header->boundary_bit = 2;
  }

  else if( header->type == 29 ); // FU-B | not supported

  return VALID_HEADER;
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

    char* stream_frame = (char*)malloc(65534*35);
    char* nal_unit = (char*)malloc(65535); // stream data here means NALU
    RTP_HEADER rtp_header;
    H264Header h264_header;

    int recv_len, stream_pointer = 4, valid_packet_started = 0;

    printf("%d %d\n", stream_pointer, valid_packet_started);

    char* sps = (char*)malloc(100);
    char* pps = (char*)malloc(100);

    // sps_set aand pps_set contains length; if == 0 then sps or pps is not received
    int sps_set = 0, pps_set = 0;

    // NOTE: there might be padding indicated by the RTP header properties
    FILE* f = fopen("img.bin", "wb");
    int alma = 2;
    while(alma) {
      memset(nal_unit, 65535, 0);

      // recv_len = recvfrom(sockfd, nal_unit, 6556, 0, (struct sockaddr*)&camera_client, (socklen_t*)sizeof(camera_client));
      recv_len = recvfrom(sockfd, nal_unit, 65534, 0, NULL, NULL);
      if(recv_len < 8) puts("RTP header is missing!");

      if(ParseRTPHeader(&rtp_header, nal_unit) != 0) continue;
      if(ParseH264Header(&h264_header, nal_unit+RTP_HEADER_LEN) != 0 ) continue;


      switch(h264_header.type) {
        case 7:
          memset(sps, 0, 100);
          sps[3] = 1;
          sps_set = recv_len-RTP_HEADER_LEN+4;
          memcpy(sps+4, nal_unit+RTP_HEADER_LEN, recv_len-RTP_HEADER_LEN);
          break;

        case 8:
          memset(pps, 0, 100);
          pps[3] = 1;
          pps_set = recv_len-RTP_HEADER_LEN+4;
          memcpy(pps+4, nal_unit+RTP_HEADER_LEN, recv_len-RTP_HEADER_LEN);
          break;
      }

      // if(h264_header.type != 28 && h264_header.type != 7 && h264_header.type != 8) continue;
      if(h264_header.type != 28) continue;

      if(h264_header.boundary_bit == 1 && !valid_packet_started) {
        valid_packet_started = 1;
        memset(stream_frame, 0, 65535*35);

        if(sps_set && pps_set) {

          memcpy(stream_frame, sps, sps_set);
          memcpy(stream_frame+sps_set, pps, pps_set);

          stream_pointer += sps_set+pps_set;
          sps_set = pps_set = 0;
        }

        stream_frame[stream_pointer-1] = 0x1;
        puts("Started valid");
      }

      if(valid_packet_started) {

        #define DEC RTP_HEADER_LEN
        memcpy(stream_frame+stream_pointer, nal_unit+DEC, recv_len-DEC);
        stream_pointer += recv_len-DEC-4;

        if(h264_header.boundary_bit == 2) {
          puts("end of boundary");
          valid_packet_started = 0;
          // FILE* f = fopen("img.bin", "wb");
          fwrite(stream_frame, 1, stream_pointer, f);
          // fclose(f);
          // break;

          stream_pointer = 4;
          memset(stream_frame, 0, 65534*35);

          alma--;
        }
      }

      // break;

    }

    fclose(f);
    free(nal_unit);
    free(stream_frame);
    free(pps);
    free(sps);

    puts("END");
    exit(0);
  }


  return camera_bridge;
}
