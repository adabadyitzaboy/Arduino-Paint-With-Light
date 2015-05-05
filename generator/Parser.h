#ifndef Parser_h
#define Parser_h
#include <Arduino.h>
#include <inttypes.h>
class Parser{
public:
  Parser();
  String encode(byte r, byte g, byte b, byte t, byte s);
  String encodeSendColor(byte i);
  String encodeRemoveColor(byte i);
  String encodeStatusChange(byte i);
  boolean decode(char* str, byte len, byte &r, byte &g, byte &b, byte &t, byte &s);
  boolean decodeMessage(char* str, byte len, byte &t, byte &b);  
};

#endif


