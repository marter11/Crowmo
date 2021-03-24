#include "packet_management/update_packets.h"

// Get data from UI
void options_interface(int sockfd, struct base *b)
{
  #if DEBUG
  char* response = options_req(b, sockfd);
  char* parsed_response_meta = options_parser(response, b);
  update_allowed_methods(parsed_response_meta, allowed_methods);

  //puts("fa");
  //printf("%s", allowed_methods);

  free(response);
  free(parsed_response_meta);
  #endif
}

void describe_interface(int sockfd, struct base *b)
{
  #if DEBUG
  char* response = describe_req(b, sockfd);
  describe_parser(response, b);

  printf("%s%s", b->stream1, b->stream2);

  free(response);
  free(b->stream1);
  free(b->stream2);

  #endif
}

void setup_interface(int sockfd, struct base* b, char* stream_path)
{
  #if DEBUG
  char* response = setup_req(b, sockfd, stream_path);
  setup_parser(response, b);
  free(response);
  #endif
}

void play_interface(int sockfd, struct base* b, char* stream_path)
{
  #if DEBUG
  char* response = play_req(b, sockfd, stream_path);
  // play_parser(response, b);
  free(response);
  #endif
}
