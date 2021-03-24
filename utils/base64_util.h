#include <math.h>

// Convert lower case string to upper case
void string_upper(char* dest, char* str, int length)
{
  int i;
  for(i=0; i<length; i++) {
    dest[i] = toupper(str[i]);
  }
}

// char base64_encode(char text[]){}

#define HIGH 32
#define OUTPUTSIZE 1024

char* logic(char* text, int len)
{
  char base64_table[64] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  int add = 0;
  int i = 0;
  int current, temp;
  int push = 2;
  int remaind = 0;
  int count = 0;
  char* out = (char*)malloc(OUTPUTSIZE);

  while(1) {
    if(count == len+1) {
      break;
    }
    current = text[i];
    current = current >> push;
    temp = current << push;
    remaind = text[i] - temp;
    out[i] = base64_table[current+add];
    int temp_push = push;
    int highest = HIGH;
    int from_prev = pow(2, temp_push)/2;
    add = 0;
    while(temp_push) {
      if(remaind & from_prev) {
        add += highest;
      }
      from_prev = from_prev/2;
      highest = highest/2;
      temp_push--;
    }
    push = push+2;
    i++;
    count++;
  }
  return out;
}

void base64_encode(char text[])
{
  int len = strlen(text);
  int times = len/2;
  int l;
  char t[3];
  int t_c = 0;
  int maradek = 0;
  char* out = (char *)malloc(OUTPUTSIZE);
  int pp;
  for(l=0; l<len; l++) {
    t[t_c] = text[l];
    t_c++;

    pp = strlen(t);
    if(pp == 3 || len-l < 2) {
      if(pp < 3) {
        maradek = pp;
      }
      t_c = 0;
      char* o = logic(t, strlen(t));
      strcat(out, o);
      free(o);
      t[0] = '\0';
      t[1] = '\0';
      t[2] = '\0';
    }
  }

  for(l=0; l<maradek-1; l++) {
    strcat(out, "=");
  }

  printf("%s\n", out);
  free(out);
}
