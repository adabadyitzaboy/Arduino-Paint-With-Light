

#ifndef Sorter_h
#define Sorter_h
#include <Arduino.h>
#include <NS_Rainbow.h>
#include <inttypes.h>
#include "ServoController.h"
class Sorter{
public:
  Sorter(NS_Rainbow *ns_strip, byte servoPin, byte redLed, byte greenLed, byte blueLed);
  void setup();
  void sort(byte num_to_sort);
  void moveToAndRemove(byte idx, byte &r, byte &g, byte &b);
  void test();
  boolean debugging;
private: 
  byte servoPin; 
  ServoController hand;
  byte redLed;
  byte greenLed;
  byte blueLed;
  int swap_delay;
  int swap_complete_delay;
  boolean bubble_sort(byte num_to_sort);
  boolean swap(uint32_t rgb1, uint32_t rgb2);
  byte get_section(byte r, byte g, byte b);
  void swap_colors(byte c1, uint32_t rgb1, byte c2, uint32_t rgb2);
  NS_Rainbow *ns_strip;
  void set_color(byte strip_cell, uint32_t led_rgb, uint32_t strip_rgb);
  void PrintValues();
};
#endif


