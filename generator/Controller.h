#ifndef Controller_h
#define Controller_h
#include <Arduino.h>
#include <inttypes.h>
class Controller{
public:
  Controller(byte LightBulbButtonPin, byte YesPin, byte NoPin, byte LinePin, byte CirclePin, byte RectanglePin, byte RoundRectanglePin, byte TrianglePin);
  void setup();
  bool YesPressed();
  bool NoPressed();
  bool BulbPressed();
  int ShapePressed();
  bool FakeInput;
private:
  bool FakePressed();
  bool Pressed(byte pin);
  byte LightBulbButtonPin;
  byte YesPin;
  byte NoPin;
  byte LinePin;
  byte CirclePin;
  byte RectanglePin;
  byte RoundRectanglePin;
  byte TrianglePin;

};

#endif

