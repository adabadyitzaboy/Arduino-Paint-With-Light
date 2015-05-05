#include "ServoController.h"

ServoController::ServoController(int servoPin): 
servoPin(servoPin){
  offset_value = 45;
  delay_value = 250;
  max_distance = 50;
  travel_distance_delay = 150;

  int p[12][12] = 
  {
    // 1  2    3  4   5   6   7   8   9   10  11  12
    { 4, 14, 23, 33, 35, 44, 54, 62, 71, 80, 89, 98},//1
    { 4, 14, 23, 33, 41, 44, 54, 62, 71, 80, 89, 98},//2
    { 4, 14, 23, 33, 41, 50, 54, 62, 71, 80, 89, 98},//3
    { 4, 14, 23, 33, 41, 50, 60, 62, 71, 80, 89, 98},//4
    { 4, 14, 23, 33, 41, 50, 60, 62, 71, 80, 90, 98},//5
    { 4, 14, 23, 33, 41, 50, 60, 69, 71, 80, 90, 98},//6
    { 4, 14, 23, 33, 41, 50, 60, 69, 77, 80, 90, 98},//7
    { 4, 14, 23, 33, 41, 50, 60, 69, 77, 85, 90, 98},//8
    { 4, 14, 23, 33, 41, 50, 60, 69, 77, 85, 93, 102},//9
    { 4, 14, 23, 33, 41, 50, 60, 69, 77, 85, 93, 102},//10
    { 4, 14, 23, 33, 41, 50, 60, 69, 77, 85, 93, 102},//11
    { 4, 14, 23, 33, 41, 50, 60, 69, 77, 85, 93, 102}//12
  };//first array is going from 59ll x.  
  //second array is going to cell y.
  for(int i= 0;i < 12;i++){
    for(int j= 0;j < 12;j++){
      servoPositions[i][j] = p[i][j];
    }
  }

  prev_cell = 1;
}
void ServoController::setup(){
  pinMode(servoPin, OUTPUT);
  attach();
  servo.write(servoPositions[0][0]);
  delay(50);
  detach();
}
void ServoController::test(){
  attach();
  servo.write(45);
  delay(500);
  servo.write(0);
  delay(500);
  servo.write(servoPositions[0][11]);
  delay(1000);
  servo.write(servoPositions[0][11]);
  delay(500);
  detach();
}
void ServoController::moveTo(int cell){
  // Serial.print("Move to - ");
  // Serial.println(cell);
  if(cell - prev_cell >= 3){
    _MoveTo(cell);//move to it twice.
    delay(50);
  }
  _MoveTo(cell);
  delay(delay_value);
}

void ServoController::_MoveTo(int cell){
  int value = servoPositions[prev_cell][cell];
  int currentDegree = servo.read();
  int travel_distance = abs(currentDegree - value);
  if(cell - prev_cell < 3){
    servo.write(value + offset_value);
    delay(delay_value);
  }
  else if(travel_distance >= max_distance){
    if(currentDegree > value){
      servo.write(value + travel_distance/2);
    }
    else{
      servo.write(value - travel_distance/2);
    }
    delay(travel_distance_delay);
  }
  // Serial.print("value - ");
  // Serial.println(value);
  servo.write(value);
  prev_cell = cell;
}
void ServoController::attach(){
  servo.attach(servoPin);
}
void ServoController::detach(){
  servo.detach();
}




