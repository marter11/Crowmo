
struct header {
  char* service_path;
  char* method;
  char version[4];
  char proto[5];
};

struct body {
  uint64_t cseq;
  uint64_t content_length;
  char* range;
  char* session;
  char* content_type;
  char* date;
  uint8_t method_code;
};

struct packet {
  uint64_t length;
  char* data;
  char* head;
  char* body;
};

#define METHODS_COUNT 11

// Request types
#define OPTIONS_CODE 1
#define DESCRIBE_CODE 2
#define SETUP_CODE 3
#define TEARDOWN_CODE 4
#define PLAY_CODE 5
#define PAUSE_CODE 6
#define RECORD_CODE 7
#define ANNOUNCE_CODE 8
#define GET_PARAMETER_CODE 9
#define SET_PARAMETER_CODE 10
#define REDIRECT_CODE 11

// Initialize allowed RTSP methods struct
struct methods {
  int OPTIONS;
  int DESCRIBE;
  int SETUP;
  int TEARDOWN;
  int PLAY;
  int SET_PARAMETER;
  int GET_PARAMETER;
  int REDIRECT;
  int PAUSE;
  int ANNOUNCE;
  int RECORD;
} allowed_methods = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// Structure for stored and default values
struct base {
  char* server_name;
  char* user_agent;
  char charset[8];
  char language[5];
  char* supported;
  char* auth_token;
  char host[16];
  uint16_t port;
  char* stream1;
  char* stream2;
  bridge* camera_bridge;
};
