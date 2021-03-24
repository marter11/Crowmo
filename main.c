
//
// Compile: gcc main.c -lm -lpthread
//

#pragma 1

// Software parameters

#define USER_AGENT "crowmo"
#define ACCEPT_LANGUAGE "en-GB"
#define ACCEPT_CHARSET "UTF-8"

// Development
#define DEBUG 1

#if DEBUG
#define HOST "192.168.0.113"
#define PORT 554
#endif

static int Cseq = 1;

#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <signal.h>
#include <arpa/inet.h>
#include <string.h>
#include <ctype.h>
#include "utils/base64_util.h"
#include "init.h"
#include "packet_management/craft_packets.h"
#include "packet_management/pack.h"
#include "requests.h"
#include "parsers/parsers.h"
#include "interfaces.h"

//DEBUG: strcpy, sprintf buffor overflow vulnerability check it and if it is neccessary make corrections!
void main(int argc, char* argv[])
{

  // Get credentials for camera if any
  #if DEBUG
  char* auth_token = (char*)malloc(120);
  strcpy(auth_token, "YWRtaW46dmFsaTE5NzUK");
  #else
  char* auth_token = (char*)malloc(120);
  char credentials[80];
  char uname[21];

  printf("Username: ");
  fgets(uname, 20, stdin);

  strcat(credentials, uname);
  strcat(credentials, ":");

  puts("");
  char* password = getpass("Password:");

  strcat(credentials, password);
  // base64_encode(credentials);

  printf("%s\n", credentials);
  #endif

  // Get camera's addresses
  #if DEBUG
  printf("DEBUG HARD CODED: %s %d\n", HOST, PORT);
  struct base *b = craft_base(USER_AGENT, ACCEPT_CHARSET, ACCEPT_LANGUAGE, "crowmo", auth_token, HOST, PORT, NULL);
  #else
  printf("Host ip address: ");
  fgets(provided_host, 15, stdin);
  printf("Port: ");
  scanf("%d", &provided_port);
  printf("DEBUG PROVIDED: %s%d\n", provided_host, provided_port);
  struct base *b = craft_base(USER_AGENT, ACCEPT_CHARSET, ACCEPT_LANGUAGE, "crowmo", auth_token, provided_host, provided_port, NULL);
  #endif

  int sockfd = init_connection(b->host, b->port);
  char command[21];

  if(sockfd) {
    while(1) {
      printf("> ");
      scanf("%20s", command);

      if(strcmp(command, "help") == 0) {
        puts("Basic calls: options, describe, setup, play");
      }

      else if(strcmp(command, "settings") == 0) {}

      else if(strcmp(command, "options") == 0) {
        options_interface(sockfd, b);
      }

      else if(strcmp(command, "describe") == 0) {
        describe_interface(sockfd, b);
      }

      else if(strcmp(command, "setup") == 0) {
        // char chosen_stream;
        // printf("Choose stream: ");
        // chosen_stream = getchar();

        // if(chosen_stream == '0') {
        //
        // }
        // else if(chosen_stream == '1') {
        //
        // }
        //
        // else {
        //   printf("NO STREAM");
        //   break;
        // }

        bridge* camera_bridge = camera_feed_bridge();
        b->camera_bridge = camera_bridge;
        setup_interface(sockfd, b, "trackID=0");
      }

      else if(strcmp(command, "play") == 0) {
        if(b->camera_bridge == NULL) {
          puts("Client-server connection not established.");
          puts("Run the 'setup' command to do this.");
        } else {
          play_interface(sockfd, b, "trackID=0");
        }
      }

      else if(strcmp(command, "quit") == 0) {
        free(auth_token);
        close(sockfd);

        free(b->user_agent);

        if(b->camera_bridge != NULL) {
          close(b->camera_bridge->sockfd);
          // kill(b->camera_bridge->pid, SIGKILL);
          free(b->camera_bridge->session);
          free(b->camera_bridge);
        }
        free(b);

        break;
      }

      // Increment cseq after every request
      Cseq++;
      memset(command, 0, 20);
    }
  }
}
