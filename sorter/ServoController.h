
#ifndef Servo_Controller_h
#define Servo_Controller_h

#include <Arduino.h>
#include <Servo.h>
#include <UtilTime.h>

class ServoController {
public:
  ServoController(int servoPin);
  void setup();
  void moveTo(int cell);
  void attach();
  void detach();
  void test();
private:
  void _MoveTo(int cell);
  Servo servo;
  int servoPin;
  int prev_cell;
  int delay_value;
  int offset_value;
  int max_distance;
  int travel_distance_delay;
  int servoPositions[12][12];

};


#endif


