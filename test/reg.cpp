#include<iostream>
#include<regex>

// const char* packet = "\x80\x60\x00\x01\x00\x00\x69\xdc\x6b\x8b\x45\x67";

typedef struct {
  unsigned char padding;
  unsigned char csrc_count;
  unsigned char marker;
  unsigned short int sequence_number;
  unsigned int timestamp;
} RTP_HEADER;

// Currently only support Fragmented Units to be processed
void ParseH264Header()
{
  char packet[1] = {0x5c};

  // FU indicator
  unsigned char forbidden_zero_bit = packet[0]>>7;
  if(forbidden_zero_bit); // drop packet

  unsigned char type = packet[0]<<3;
  type = type>>3;

  if( type == 28 ); // FU-A
  else if( type == 29 ); // FU-B | not supported

  printf("%d\n", type);

}

int main()
{
  // unsigned char packet[8] = {0xaa, 0x80, 0x01, 0x10, 0x10, 0x00, 0x00, 0x00};
  char packet[] = "\x80\x60\x00\x01\x00\x00\x69\xdc\x6b\x8b\x45\x67";

  // Version (supported specification is 2)
  if( !(packet[0]&0x80) ) return -1;

  // Padding bit
  unsigned char padding = (packet[0]>>5) - 0b100;

  // CSRC count (CC)
  unsigned char csrc_count = (packet[0]<<4);
  csrc_count=csrc_count>>4;

  // Marker bit
  unsigned char marker = packet[1]>>7;

  // Payload type: ignore for now

  unsigned short int sequence_number = packet[2];
  sequence_number = sequence_number<<8;
  sequence_number += packet[3];

  unsigned int timestamp = packet[4];
  timestamp = (timestamp<<24);
  timestamp += (0x00+packet[5])<<16;
  timestamp += (0x00+packet[6])<<8;
  timestamp += packet[7];

  // SSRC and CSRC list is not implemented

  // printf("%d\n", marker);
  // printf("%d\n", b>>6);

  ParseH264Header();

}
