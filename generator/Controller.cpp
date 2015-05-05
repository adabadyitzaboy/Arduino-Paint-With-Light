#include "Controller.h"
Controller::Controller(byte LightBulbButtonPin, byte YesPin, byte NoPin, byte LinePin, byte CirclePin, byte RectanglePin, byte RoundRectanglePin, byte TrianglePin)
:
LightBulbButtonPin(LightBulbButtonPin), YesPin(YesPin), NoPin(NoPin), LinePin(LinePin), CirclePin(CirclePin), RectanglePin(RectanglePin), RoundRectanglePin(RoundRectanglePin), TrianglePin(TrianglePin), FakeInput(false){

}
void Controller::setup(){
  pinMode(LightBulbButtonPin, INPUT);
  pinMode(YesPin, INPUT);
  pinMode(NoPin, INPUT);
  pinMode(LinePin, INPUT);
  pinMode(CirclePin, INPUT);
  pinMode(RectanglePin, INPUT);
  pinMode(RoundRectanglePin, INPUT);
  pinMode(TrianglePin, INPUT);

}
bool Controller::FakePressed(){
  Serial.println("Please enter Y or N");
  while(true){
    if (Serial.available() > 0) {
      // read the incoming byte:
      char r = (char) Serial.read();
      if(r == 'Y'){
        while(Serial.available()){
          Serial.read();
        };
        return true;
      }
      else if(r == 'N'){
        while(Serial.available()){
          Serial.read();
        };
        return false;
      }
      else{
        Serial.println("Wrong entry.  Please enter Y or N ");
      }
    }
  }
}
bool Controller::Pressed(byte pin){
  if(FakeInput){
    return FakePressed();
  }
  int state = digitalRead(pin);
  return state == HIGH;
}
bool Controller::YesPressed(){
  return Pressed(YesPin);
}
bool Controller::NoPressed(){
  return Pressed(NoPin);
}
bool Controller::BulbPressed(){
  return Pressed(LightBulbButtonPin);
}
int Controller::ShapePressed(){
  if(FakeInput){
    Serial.println("Please enter number between 1 and 5.");
    while(true){
      if(Serial.available()){
        char num = Serial.read();
        if((int) num >= 49 && (int) num <= 53){
          while(Serial.available()){Serial.read();}
          return ((int) num) - 48;
        }else{
          Serial.print("Invalid.");
          while(Serial.available()){Serial.read();}
        }
      }
      delay(500);
    }
  }
  byte pins[5] = {
    LinePin, CirclePin, RectanglePin, RoundRectanglePin, TrianglePin  };
  for(int idx = 0; idx < 5;idx++){
    if(Pressed(pins[idx])){
      return idx + 1;//correlates to the shape.
    }
  }
  return 0;  
}

