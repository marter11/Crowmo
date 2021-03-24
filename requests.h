#include "provider_specific/provider_spec.h"

char* request_handler(struct packet* p, int sockfd)
{
  send(sockfd, p->data, p->length, 0);

  // Recv data within 5 seconds otherwise ...
  char* recv_buffer = malloc(1024);
  struct timeval tv;
  tv.tv_sec = 5;
  setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(struct timeval));
  recv(sockfd, recv_buffer, 1024, 0);

  // DEBUG: if recv is successful with the same CSeq as sent then increment cseq and approve

  return recv_buffer;
}

char* options_req(struct base* b, int sockfd)
{
  struct header* head = craft_header("OPTIONS", "1.0", "rtsp", NULL);
  struct body* bo = craft_body(Cseq, 0, NULL, NULL, NULL, 0, OPTIONS_CODE);
  struct packet* packet_properties = pack(head, b, bo);

  char* recv_buffer = request_handler(packet_properties, sockfd);

  free(packet_properties->data);
  free(packet_properties);

  return recv_buffer;
}

char* describe_req(struct base* b, int sockfd)
{
  struct header* head = craft_header("DESCRIBE", "1.0", "rtsp", "/11");
  struct body* bo = craft_body(Cseq, 0, NULL, NULL, NULL, 0, DESCRIBE_CODE);
  struct packet* packet_properties = pack(head, b, bo);

  char* recv_buffer = request_handler(packet_properties, sockfd);

  free(packet_properties->data);
  free(packet_properties);

  return recv_buffer;
}

char* setup_req(struct base* b, int sockfd, char* stream_path)
{
  char* service_path = (char*)malloc(120);

  char s[4] = "/11";
  sprintf(service_path, "%s/%s", s, stream_path);

  struct header* head = craft_header("SETUP", "1.0", "rtsp", service_path);
  struct body* bo = craft_body(Cseq, 0, NULL, NULL, NULL, 0, SETUP_CODE);
  struct packet* packet_properties = pack(head, b, bo);

  char* recv_buffer = request_handler(packet_properties, sockfd);

  free(service_path);
  free(packet_properties->data);
  free(packet_properties);

  return recv_buffer;
}

char* play_req(struct base* b, int sockfd, char* stream_path)
{
  char* service_path = (char*)malloc(120);

  char s[4] = "/11";
  sprintf(service_path, "%s/%s", s, stream_path);

  struct header* head = craft_header("PLAY", "1.0", "rtsp", service_path);
  struct body* bo = craft_body(Cseq, 0, NULL, NULL, NULL, 0, PLAY_CODE);
  struct packet* packet_properties = pack(head, b, bo);

  char* recv_buffer = request_handler(packet_properties, sockfd);

  free(service_path);
  free(packet_properties->data);
  free(packet_properties);

  return recv_buffer;
}
