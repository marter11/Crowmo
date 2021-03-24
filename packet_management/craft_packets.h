#include "packets.h"

struct header *craft_header(char s_method[], char s_version[4], char s_proto[5], char service_path[])
{
  struct header* head = malloc(sizeof(struct header));

  //DEBUG: Set malloc to relative size
  char* method = malloc(50);

  strcpy(method, s_method);
  strcpy(head->version, s_version);
  strcpy(head->proto, s_proto);
  head->method = method;
  head->service_path = service_path;

  return head;
}

struct base *craft_base
(char s_user_agent[], char charset[8],
char language[5], char supported[],
char* auth_token, char host[],
uint16_t port, bridge* camera_bridge)
{
  struct base* b = malloc(sizeof(struct base));

  //DEBUG: specify relative size for s_user_agent

  char* user_agent = malloc(20);

  strcpy(user_agent, s_user_agent);
  strcpy(b->language, language);
  strcpy(b->host, host);
  strcpy(b->charset, charset);
  b->user_agent = user_agent;
  b->port = port;
  b->auth_token = auth_token;
  b->camera_bridge;

  return b;
}

struct body *craft_body
(uint64_t cseq, uint64_t content_length,
char* range, char* session,
char* content_type, int date, uint8_t method_code)
{

  struct body* bo = malloc(sizeof(struct body));

  bo->cseq = cseq;
  bo->method_code = method_code;

  if(content_length != 0) {
    bo->content_length = content_length;
  }

  if(range != NULL) {
    bo->range = range;
  }

  if(session != NULL) {
    bo->session = session;
  }

  if(content_type != NULL) {
    bo->content_type = content_type;
  }

  if(date) {
    // Get the current date
    bo->date = "Jan 08";
  }

  return bo;
}
