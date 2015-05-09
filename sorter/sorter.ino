//Euton/Rowe Project 3b - Color Sorter Model
// toneAC uses two pins but is twice as loud as tone
#include <SPI.h>
#include <NS_Rainbow.h>
#include <Servo.h>
#include <RF24.h>
#include <nRF24L01.h>
#include "Radio.h"
#include "Parser.h"
#include "SortRoutine.h"

//define pinout
#define tonePin 7
#define redLed 3
#define greenLed 4
#define blueLed 5
#define LED_STRIP_PIN 2  //output for the led strip
#define HAND_PIN 6 //output for servo
#define WIFI_CEPin 9
#define WIFI_CSNPin 10
#define DebugTestPin 8
#define N_CELL 12  //number of cells to light up in the strip.
#define GeneratorChannel 1
#define MatrixChannel 2
#define RadioRole 1


//Define enumerations
typedef enum { 
  reading = 1, ready_to_sort = 2, start_sorting = 3, sorting = 4, sort_complete = 5, all_done = 6, reset = 7} 
status_e;
typedef enum { 
  send_color = 1, remove_color = 2, change_status = 3} 
message_type_e;

//Global variables
status_e status = reading;
byte shapes[N_CELL];
int color_index = 0;//tracks which led in the strip is next to be lit.
bool debugging = false;//tracks if we are in debug mode.
int num_sorted = 0;

//Create objects
NS_Rainbow ns_strip = NS_Rainbow(N_CELL,LED_STRIP_PIN);//Led Strip
SortRoutine sorter(&ns_strip, HAND_PIN, redLed, greenLed, blueLed);
Parser parser = Parser();//encodes and decodes the values.
Radio radio(2, WIFI_CEPin, WIFI_CSNPin);

void setup() {
  //
  // Print preamble
  //

  Serial.begin(57600);
  Serial.print("Starting: ");
  //
  // Setup light strip
  //
  ns_strip.begin();
  ns_strip.clear();
  ns_strip.setBrightness(255);  // range: 0 ~ 255

    //setup the tone pin
  pinMode(tonePin, OUTPUT);

  radio.setup();
  sorter.setup();
  pinMode(DebugTestPin, INPUT);
  if(digitalRead(DebugTestPin) == HIGH){
    debugging = true;
    sorter.debugging = true;
  }
  SetStripColor(0, 0, 0);
}
void reset_all(){ 
  radio.changeWritingChannel(MatrixChannel);
  while(radio.send(parser.encodeStatusChange(reset))!= 1){ delay(500);}
  radio.changeWritingChannel(GeneratorChannel);      
  status = reading;
  color_index = 0;
  num_sorted = 0;
  SetStripColor(0, 0, 0);
}
void radio_loop(void)
{
  byte len;
  char* str = radio.receive(len);
  if(len > 0){
    //output the color
    byte rgb[3];
    byte color_tone;
    byte shape;
    if(len < 11){
      byte message = 0;
      byte messageType = 0;
      if(parser.decodeMessage(str, len, messageType, message)){      
        if(debugging){
          Serial.print("Received Message");
          Serial.println(str);
        }
        if(messageType == send_color || messageType ==  remove_color){
          int complete_status = 1;
          if(color_index == num_sorted - 1){
            complete_status = 2;
          }
          while(!radio.sendResponse(complete_status)){
            delay(500);
          }
          if(debugging){
            Serial.println("Removing color");
          }
          //remove the color from this strip.
          sorter.moveToAndRemove(message, rgb[0], rgb[1], rgb[2]);

          if(messageType == send_color){
            //send the color to the matrix
            if(debugging){
              Serial.println("Sending color to matrix");
            }
            radio.changeWritingChannel(MatrixChannel);

            int success = 0;
            while(success != 42){
              success = radio.send(parser.encode(rgb[0], rgb[1], rgb[2], 0, shapes[message% N_CELL]));
              if(success != 42){
                delay(500);
              }
            }
            if(debugging){
              Serial.println("color sent");
            }
            radio.changeWritingChannel(GeneratorChannel);
          }

          if(debugging){
            Serial.println("Sending done message");
          }
          while(!radio.send(1)){//done
            delay(500);
          }
          color_index++;
          if(complete_status == 2){
            Serial.println("All Done!");
            status = all_done;
          }
        }
        else if(messageType ==  change_status && message == start_sorting){
          while(!radio.sendResponse(sorting)){
            delay(500);
          }
          status = sorting;
        }
        else if(messageType == change_status && message == reset){
          while(!radio.send(1)){//done
            delay(500);
          }
          reset_all();
        }          
      }
    }
    else if(status != sort_complete && parser.decode(str, len, rgb[0], rgb[1], rgb[2], color_tone, shape)){
      status = reading;
      if(color_index == ns_strip.numCells() - 2){
        status = ready_to_sort;
      }
      while(!radio.sendResponse(status)){
        delay(500);
      }

      tone(tonePin, color_tone, 500);
      Serial.print("r-");
      Serial.print(rgb[0]);
      Serial.print("g-");
      Serial.print(rgb[1]);
      Serial.print("b-");
      Serial.println(rgb[2]);

      shapes[color_index] = shape;      
      ns_strip.setColor(color_index++ % ns_strip.numCells(), rgb[0], rgb[1], rgb[2]);
      ns_strip.show();

    }
    else{
      Serial.print("Failed read\r\nLength-");
      Serial.print(len);
      Serial.println(str);
    }
  }
}
void SetStripColor(byte red, byte green, byte blue){  
  for(byte idx = 0; idx < ns_strip.numCells(); idx++){    
    ns_strip.setColor(idx, red, green, blue);
  }
  ns_strip.show();
}
void sort(){
  Serial.println("starting sort");
  num_sorted = color_index;
  sorter.sort(num_sorted);
  Serial.println("sort finished");
  while(!radio.send(sort_complete)){
    delay(500);
  }
  status = sort_complete;
  color_index = 0;
}
void sortComplete(){
  //NOOP
  radio_loop();  
}
void AllDone(){
  byte len;
  char* str = radio.receive(len);
  if(len > 0){
    byte message = 0;
    byte messageType = 0;
    if(parser.decodeMessage(str, len, messageType, message)){
      if(messageType == change_status){
        if(message == reset){
          while(!radio.send(1)){//done
            delay(500);
          }   
          reset_all();
        }
      }
    }
    else{
      Serial.print("Failed read\r\nLength-");
      Serial.print(len);
      Serial.println(str);
    }
  }
  else{
    delay(1000);
  }
}

void Menu(void)
{
  if(status == reading || status == ready_to_sort){
    radio_loop();
  }
  else if(status == sorting){
    sort();
  }
  else  if(status == sort_complete){
    sortComplete();
  }
  else if(status == all_done){
    AllDone();
  }
}
void loop(){
  if(debugging && false){
    if(digitalRead(DebugTestPin) == LOW){
      debugging = false;
      sorter.debugging = false;
    }    
    SetStripColor(255, 0, 0);
    delay(1000);
    SetStripColor(0, 255, 0);
    delay(1000);
    SetStripColor(0, 0, 255);
    delay(1000);
    sorter.test();
    delay(1000);
  }
  else{
    Menu();
  }
}

