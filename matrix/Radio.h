#ifndef Radio_h
#define Radio_h

#include <Arduino.h>
#include <inttypes.h>
#include <nRF24L01.h>
#include <RF24.h>

class Radio{
public:
  Radio(int role, int pin1, int pin2);
  void setup();
  int send(String str);
  boolean send(int status);
  boolean sendResponse(int status);
  char* receive(uint8_t &len);
  boolean receive(int &value);
  void changeWritingChannel(int channel);
private: 
  RF24 radio;// Set up nRF24L01 radio on SPI bus plus pins 9 & 10
  uint64_t pipes[4];// Radio pipe addresses for the nodes to communicate.
  static const int payload_size = 18;
  char receive_payload[payload_size];//t255-255-255-125. +1
  int role;

};

#endif


