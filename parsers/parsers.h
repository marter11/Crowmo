#define RESPONSE "parsers/exchange/response"
#define TEMP "parsers/exchange/temp"
#define PYTHON_PARSER "./parsers/parsers.py"

#define PARSED_SIZE 512

#include <fcntl.h>
#include <pthread.h>

void* options_parser(char* response, struct base* b)
{
  char code[3];

  int length = strlen(response);
  FILE* open_temp_file;

  // Transmit parasable data to temp
  open_temp_file = fopen(TEMP, "w");
  sprintf(code, "%d\n", OPTIONS_CODE);
  fwrite(code, 1, 2, open_temp_file);
  fwrite(response, 1, length, open_temp_file);
  fclose(open_temp_file);

  int sys = system(PYTHON_PARSER);

  if(sys == 0) {

    // Read parsed data from response file
    FILE* open_response_file;
    open_response_file = fopen(RESPONSE, "r");

    char* parsed_response = (char *)malloc(PARSED_SIZE);
    char* parsed_response_meta = (char *)malloc(METHODS_COUNT+1);

    fgets(parsed_response, PARSED_SIZE, open_response_file);
    fgets(parsed_response_meta, METHODS_COUNT, open_response_file);

    fclose(open_response_file);
    printf("%s", parsed_response);

    free(parsed_response);

    #if DEBUG
    system("echo '' > parsers/exchange/response");
    system("echo '' > parsers/exchange/temp");
    #endif

    return parsed_response_meta;
  }

  else {
    puts("Error during python parser execution!");
  }
}

void describe_parser(char* response, struct base* b)
{
  FILE* open_temp_file;
  int length;
  char code[3];

  open_temp_file = fopen(TEMP, "w");
  length = strlen(response);

  sprintf(code, "%d\n", DESCRIBE_CODE);
  fwrite(code, 1, 2, open_temp_file);
  fwrite(response, 1, length, open_temp_file);
  fclose(open_temp_file);

  int sys = system(PYTHON_PARSER);

  if(sys == 0) {
    FILE* open_response_file;
    int stream_count;
    char stream_count_string[3];

    open_response_file = fopen(RESPONSE, "r");
    fgets(stream_count_string, 3, open_response_file);
    stream_count = atoi(stream_count_string);


    // More than 2 stream is currently not supported
    int i;
    stream_count = 2;
    for(i=0; i<stream_count; i++) {
      if(i == 0) {
        char* stream1 = (char*)malloc(50);
        fgets(stream1, 48, open_response_file);
        b->stream1 = stream1;
      }

      else {
        char* stream2 = (char*)malloc(50);
        fgets(stream2, 48, open_response_file);
        b->stream2 = stream2;
      }
    }

    fclose(open_response_file);
  }
}

void setup_parser(char* response, struct base* b)
{
  char code[3];
  int length = strlen(response);
  FILE* open_temp_file;

  // Transmit parasable data to temp
  open_temp_file = fopen(TEMP, "w");
  sprintf(code, "%d\n", SETUP_CODE);
  fwrite(code, 1, 2, open_temp_file);
  fwrite(response, 1, length, open_temp_file);
  fclose(open_temp_file);

  int sys = system(PYTHON_PARSER);

  if(sys == 0) {

    // Read parsed data from response file
    FILE* open_response_file;
    open_response_file = fopen(RESPONSE, "r");

    char* session = (char *)malloc(41);
    fgets(session, 40, open_response_file);
    b->camera_bridge->session = session;

    fscanf(open_response_file, "%hd", &(b->camera_bridge->server_port));

    fclose(open_response_file);

    printf("%s%d\n", b->camera_bridge->session, b->camera_bridge->server_port);

    #if DEBUG
    system("echo '' > parsers/exchange/response");
    system("echo '' > parsers/exchange/temp");
    #endif

  } else {
    puts("Error during python parser execution!");
  }
}
