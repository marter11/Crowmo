
void pack_header
(struct header *head, char* host,
uint16_t port, struct packet* properties)
{
  char* buffer = (char *)malloc(sizeof(head->method)+sizeof(head->proto)+16+sizeof(port)+sizeof(head->service_path)+2);

  if(port) {
    sprintf(buffer, "%s %s://%s:%d", head->method, head->proto, host, port);
  }

  // else {
    // sprintf(buffer, "%s %s://%s/%s ", head->method, head->proto, host, head->service_path);
  // }

  if(head->service_path != NULL) {
      strcat(buffer, head->service_path);
      strcat(buffer, " ");
  }

  char proto_version_string[10];
  char proto_uppercase[5];

  string_upper(proto_uppercase, head->proto, sizeof(head->proto));
  sprintf(proto_version_string, "%s/%s", proto_uppercase, head->version);
  strcat(buffer, proto_version_string);
  strcat(buffer, "\r\n");

  properties->length += strlen(buffer);
  properties->head = buffer;

  // printf("%s Length: %ld\n", buffer, strlen(buffer));
}

void pack_body(struct base* ba, struct body* bo, struct packet* properties)
{

  char* buffer = (char *)malloc(sizeof(struct base)+sizeof(struct body)+200);
  // sprintf(buffer, "CSeq: %ld\r\nAuthorization: Basic %s\r\n", bo->cseq, ba->auth_token);
  sprintf(buffer, "CSeq: %ld\r\n", bo->cseq);

  #define camera_bridge ba->camera_bridge

  switch (bo->method_code) {
    case DESCRIBE_CODE:
      strcat(buffer, "Authorization: Basic ");
      strcat(buffer, ba->auth_token);
      strcat(buffer, "\r\n");
      break;
    case SETUP_CODE:
      if(camera_bridge != NULL) {
        if(camera_bridge->active) {
          strcat(buffer, "Authorization: Basic ");
          strcat(buffer, ba->auth_token);
          strcat(buffer, "\r\n");
          #define bridge_port camera_bridge->client_port
          sprintf(buffer+strlen(buffer), "Transport: RTP/AVP;unicast;client_port=%d-%d\r\n", bridge_port, bridge_port+1);
          #undef bridge_port
        } else {

        }
      }
      break;
    case PLAY_CODE:
      strcat(buffer, "Authorization: Basic ");
      strcat(buffer, ba->auth_token);
      strcat(buffer, "\r\n");

      // TODO: Make range choosable
      sprintf(buffer+strlen(buffer), "Session: %s\r\nRange: %s\r\n", camera_bridge->session, "10-25");
      break;
    default:
      sprintf(buffer, "CSeq: %ld\r\n", bo->cseq);
      break;
  }

  #undef camera_bridge

  // Notice: It may be usefull to support the accept-charset, etc. announcement
  // sprintf(buffer, "CSeq: %ld\r\nAccept-Charset: %s\r\nUser-Agent: %s\r\nAuthorization: Basic %s\r\n\r\n", bo->cseq, ba->charset, ba->user_agent, ba->auth_token);

  // DEBUG: comment out the strcat then handle the error when server doesen't send response
  strcat(buffer, "\r\n");

  properties->length += strlen(buffer);
  properties->body = buffer;
}

struct packet* pack(struct header *head, struct base *b, struct body *bo)
{

  struct packet* packet_properties = (struct packet *)malloc(sizeof(struct packet)+2);
  packet_properties->length = 0;

  pack_header(head, b->host, b->port, packet_properties);
  pack_body(b, bo, packet_properties);

  char* buffer = (char *)malloc(packet_properties->length);
  strcpy(buffer, packet_properties->head);
  strcat(buffer, packet_properties->body);
  strcat(buffer, "\r\n");

  // Because \r and \n also 2 bytes
  packet_properties->length += 2;

  // Head and body data are combined in packet_properties->data
  free(packet_properties->head);
  free(packet_properties->body);
  free(head->method);
  free(head);
  free(bo);

  packet_properties->data = buffer;
  return packet_properties;
}
