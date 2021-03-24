
void update_allowed_methods(char input[METHODS_COUNT], struct methods allowed_methods)
{
  int time = 0;
  int method_code;

  while(time < METHODS_COUNT) {
    method_code = input[time]-48;

    switch(method_code) {
      case 0: break;
      case OPTIONS_CODE: allowed_methods.OPTIONS = 1; break;
      case DESCRIBE_CODE: allowed_methods.DESCRIBE = 1; break;
      case SETUP_CODE: allowed_methods.SETUP = 1; break;
      case TEARDOWN_CODE: allowed_methods.TEARDOWN = 1; break;
      case PLAY_CODE: allowed_methods.PLAY = 1; break;
      case PAUSE_CODE: allowed_methods.PAUSE = 1; break;
      case RECORD_CODE: allowed_methods.RECORD = 1; break;
      case ANNOUNCE_CODE: allowed_methods.ANNOUNCE = 1; break;
      case GET_PARAMETER_CODE: allowed_methods.GET_PARAMETER = 1; break;
      case SET_PARAMETER_CODE: allowed_methods.SET_PARAMETER = 1; break;
      case REDIRECT_CODE: allowed_methods.REDIRECT = 1; break;
    }
    time++;
  }
}

void update_available_streams()
{
  
}
