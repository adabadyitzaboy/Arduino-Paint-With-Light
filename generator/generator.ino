//Euton/Rowe Project 3b - Color Generater Model
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Adafruit_NeoPixel.h>
#include <LiquidCrystal.h>
#include "Radio.h"
#include "Parser.h"
#include "Controller.h"

//define pinout
#define LightBulbPin 21
#define LightBulbPin2 22
#define LightBulbPin3 23
#define LightBulbButtonPin 39
#define tonePin 6
#define YesPin 37
#define NoPin 38
#define LinePin 32
#define CirclePin 34
#define RectanglePin 35
#define RoundRectanglePin 33
#define TrianglePin 36
#define WIFI_CEPin 9
#define WIFI_CSNPin 10
#define LCD_RS 7
#define LCD_ENABLE 8
#define LCD_D0 40
#define LCD_D1 41
#define LCD_D2 42
#define LCD_D3 43
#define LCD_D4 28
#define LCD_D5 29
#define LCD_D6 30
#define LCD_D7 31
#define LCD_Red 44
#define LCD_Green 45
#define LCD_Blue 46

//define delays, etc.
#define output Serial.print
#define WaitToStartLoopDelay 2000
#define ChooseColorAndShapeLoopDelay 1500
#define outputMessageDelay 2000
#define chooseColorDelay 2000
#define RadioRole 1
#define LCD_NumColumns 20
#define LCD_NumRows 2
#define brightness 100
#define DebugTestPin A0

//Define enumerations
typedef enum { 
  reading = 1, ready_to_sort = 2, start_sorting = 3, sorting = 4, sort_complete = 5, all_done = 6, reset = 7} 
status_e;
typedef enum { 
  waiting_to_start = 1, choosing_color_and_shape = 2, waiting_on_sort = 3, painting = 4, paint_complete = 5 } 
status_artist_e;
typedef enum { 
  line = 1, circle = 2, rectangle = 3, round_rectangle = 4, triangle = 5 } 
shapes_e;

//define global variables
status_artist_e artist_status = waiting_to_start;//controls the menu
unsigned long tickCounter = 0;//tracks the time since the last activity.
int SubRoutineStep = 0;//tracks which submenu we are in.
byte shape_index = line;//tracks which shape was pressed last.
int color_index = 0;//tracks which led in the strip is next to be lit.
int num_painted = 0;//tracks the number of colors painted.
bool sorter_reset = true;

//define global constants
const byte red[] =   		{255,255,255,255,128,  0,  0,  0,100,255};     // red component
const byte green[] = 		{  0, 45,128,255,255,255,128,  0,  0,  0};   // green component
const byte blue[] =  		{  0,  0,  0,  0,  0,  0,255,255,255,128};    // blue component
const int colorTone[] = {196,220,247,262,294,330,349,392,440,494};  // Audio tones corresponding to the colors
const char* selected_output_message[] = {"Wonderful", "Beautiful", "Very well", "Good", "A good choice"};
const int numColors = sizeof(red) / sizeof(red[0]);     // Number of colors to be sorted
const int selected_output_message_length = 5;//number of messages in selected_output_message;
bool debugging = false;


//Create objects
LiquidCrystal lcd(LCD_RS, LCD_ENABLE, LCD_D0, LCD_D1, LCD_D2, LCD_D3, LCD_D4,LCD_D5,LCD_D6,LCD_D7);//RGB LCD
Adafruit_NeoPixel jewel = Adafruit_NeoPixel(7, LightBulbPin);
Adafruit_NeoPixel ring12 = Adafruit_NeoPixel(12, LightBulbPin2);
Adafruit_NeoPixel ring16 = Adafruit_NeoPixel(18, LightBulbPin3);
Radio radio(RadioRole, WIFI_CEPin, WIFI_CSNPin);// Set up nRF24L01 radio on SPI bus plus pins 9 & 10
Parser parser = Parser();//encodes and decodes the values.
Controller controller = Controller(LightBulbButtonPin, YesPin, NoPin, LinePin, CirclePin, RectanglePin, RoundRectanglePin, TrianglePin);//determines what is pressed.

void setup() 
{

  // Initialize Neopixels to off
  jewel.begin();
  jewel.show();
  ring12.begin();
  ring12.show();
  ring16.begin();
  ring16.show();

  //
  // Print preamble
  //

  Serial.begin(57600);
  Serial.println("Starting:");

  //setup random
  //  randomSeed(42); 

  radio.setup();
  controller.setup();

  pinMode(LCD_Red, OUTPUT);
  pinMode(LCD_Green, OUTPUT);
  pinMode(LCD_Blue, OUTPUT);
  lcd.begin(LCD_NumColumns,LCD_NumRows);

  pinMode(DebugTestPin, INPUT);
  if(digitalRead(DebugTestPin) == HIGH){
    debugging = true;
    controller.FakeInput = true;
    Serial.println("Debugging");
  }
  setBacklight(0,255,0);
  Serial.println("Setup Complete");
}
void WriteMessage(String message){
  lcd.clear();
  lcd.setCursor(0, 0);
  int new_line_index = message.indexOf('\n');
  if(new_line_index != -1){
    int row = 1;
    int start = 0;
    while(new_line_index != -1){
      lcd.print(message.substring(start, start + new_line_index));
      lcd.setCursor(0, row++);
      if(debugging){
        Serial.print(message.substring(start, start + new_line_index));
        Serial.print(" ");
        Serial.println(new_line_index);
      }
      start += new_line_index + 1;
      new_line_index = message.substring(start).indexOf('\n');
    }
    lcd.setCursor(0, row);
    lcd.print(message.substring(start));
    if(debugging){
      Serial.print(message.substring(start));
      Serial.print(" ");
      Serial.println(message.length() - start);
    }
  }
  else {    
    lcd.print(message);
    if(debugging){
      Serial.print(message);
      Serial.print(" ");
      Serial.println(message.length());
    }
  }
}
void setBacklight(uint8_t r, uint8_t g, uint8_t b) {
  // normalize the red LED - its brighter than the rest!
  r = map(r, 0, 255, 0, 100);
  g = map(g, 0, 255, 0, 150);
  r = map(r, 0, 255, 0, brightness);
  g = map(g, 0, 255, 0, brightness);
  b = map(b, 0, 255, 0, brightness);
  // common anode so invert!
  r = map(r, 0, 255, 255, 0);
  g = map(g, 0, 255, 255, 0);
  b = map(b, 0, 255, 255, 0);

  analogWrite(LCD_Red, r);
  analogWrite(LCD_Green, g);
  analogWrite(LCD_Blue, b);
}
void WaitToStartLoop(){
  if(SubRoutineStep == 0  || (SubRoutineStep == 2 && millis() - tickCounter >= WaitToStartLoopDelay)){
    WriteMessage("Bonjour!\nI am Claude, an\nartist. I like to\npaint with light.");
    tickCounter = millis();
    SubRoutineStep = 1;
  }
  else if(SubRoutineStep == 1 && millis() - tickCounter >= WaitToStartLoopDelay){
    WriteMessage("Will you help me?");
    tickCounter = millis();
    SubRoutineStep = 2;
  }
  if(controller.YesPressed()){
    WriteMessage("Magnifique!\nI will think of\nsome colors.");
    artist_status = choosing_color_and_shape;
    SubRoutineStep = 0;
    color_index = 0;
    num_painted = 0;
    delay(WaitToStartLoopDelay);
    if(!sorter_reset){
      while(radio.send(parser.encodeStatusChange(reset)) != 1){
        delay(500);
      }
    }
  }
}
void IlluminateBulb(bool illuminate){
  //illuminate the bulb to the color_index
  if(illuminate){
    int idx = color_index % numColors;
    for(int jewel_index = 0; jewel_index < jewel.numPixels(); jewel_index++){
      jewel.setPixelColor(jewel_index,red[idx],green[idx],blue[idx]);    
      delay(20);  
    }
    jewel.show();// Turn it on 
    for(int ring12_index = 0; ring12_index < ring12.numPixels(); ring12_index++){
      ring12.setPixelColor(ring12_index,red[idx],green[idx],blue[idx]);    
      delay(20);  
    }
    ring12.show();// Turn it on
    for(int ring16_index = 0; ring16_index < ring16.numPixels(); ring16_index++){
      ring16.setPixelColor(ring16_index,red[idx],green[idx],blue[idx]);      
    }
    ring16.show();// Turn it on
 
    setBacklight(red[idx], green[idx], blue[idx]);
    delay(500);
    tone(tonePin, colorTone[idx],500);    // Play tone corresponding to the color for 1/2 second
    Serial.print("r-");
    Serial.print(red[idx]);
    Serial.print("g-");
    Serial.print(green[idx]);
    Serial.print("b-");
    Serial.println(blue[idx]);
  }
  else{
    jewel.clear();
    ring12.clear();
    ring16.clear();

  }
}
void ChooseColorAndShapeLoop(){
  if(SubRoutineStep == 0){
    WriteMessage("Do you like\nthis color?");
    SubRoutineStep = 1;
    IlluminateBulb(true);
    tickCounter = millis();
  }
  else if(SubRoutineStep == 1){
    //waiting for user to press the bulb
    if(controller.BulbPressed()){
      WriteMessage(selected_output_message[random(0, selected_output_message_length)]);
      SubRoutineStep = 2;
      delay(outputMessageDelay);
      IlluminateBulb(false);
    }
    else if(millis() - tickCounter >= chooseColorDelay){
      //next color
      color_index = (color_index + 1) % numColors;
      IlluminateBulb(true);
      tickCounter = millis();
    }
  }
  else if(SubRoutineStep == 2){
    WriteMessage("Choose a shape.");
    SubRoutineStep = 3;
  }
  else if(SubRoutineStep == 3){
    //waiting for user to press a shape
    shape_index = controller.ShapePressed();
    if(shape_index != 0){
      WriteMessage(selected_output_message[random(0, selected_output_message_length)]);
      SubRoutineStep = 4;
      delay(outputMessageDelay);
    }		
  }
  else if(SubRoutineStep == 4){
    //send this color and shape to the sorter.
    int sorter_status = radio.send(parser.encode(red[color_index], green[color_index], blue[color_index], colorTone[color_index], shape_index));
    if(sorter_status == reading || sorter_status == ready_to_sort){
      WriteMessage("I placed it on\nmy palette.");
      delay(outputMessageDelay);
      num_painted++;
      color_index = 0;
      
      if(sorter_status == reading){
        SubRoutineStep = 5;
        IlluminateBulb(true);      
        WriteMessage("Do you want to\nadd another?");
        delay(outputMessageDelay);
      }else{
        SubRoutineStep = 6;
      }
    }
    //else NOOP
  }
  else if(SubRoutineStep == 5){
    //Waiting for Yes to continue or No to sort
    if(controller.YesPressed()){
      WriteMessage(selected_output_message[random(0, selected_output_message_length)]);			
      SubRoutineStep = 0;
      delay(outputMessageDelay);
    }
    else if(controller.NoPressed()){
      SubRoutineStep = 6;	
      WriteMessage(selected_output_message[random(0, selected_output_message_length)]);			
      delay(outputMessageDelay);
    }
  }
  else if(SubRoutineStep == 6){
    //ready to sort
    WriteMessage("The colors are\nnot in order.");
    delay(outputMessageDelay);
    WriteMessage("I cannot\npaint this way!");
    delay(outputMessageDelay);
    WriteMessage("I will\nsort them.");
    SubRoutineStep = 7;
    delay(outputMessageDelay);
  }
  else if(SubRoutineStep == 7){
    //start sorting
    int sorter_status = radio.send(parser.encodeStatusChange(start_sorting));
    if(sorter_status == sorting){
      artist_status = waiting_on_sort;
      SubRoutineStep = 0;
    }
    else{
      delay(500);
    }
  }	
}
void WaitOnSortLoop(){
  int value;
  boolean done = radio.receive(value);
  if(done && value == sort_complete){
    artist_status = painting;
    SubRoutineStep = 0;
    while(!radio.sendResponse(1)){
      delay(500);
    }
  }
  else{
    delay(1000);
  }
}
void PaintingLoop(){
  if(SubRoutineStep ==0){
    WriteMessage("I am ready\nto paint now.");
    delay(outputMessageDelay);
    SubRoutineStep = 1;
    color_index = 0;
  }
  else if(SubRoutineStep == 1){
   WriteMessage("Shall I paint\nthis color?");
    setBacklight(red[color_index], green[color_index], blue[color_index]);
    delay(outputMessageDelay);
    SubRoutineStep = 2;
  }
  else if(SubRoutineStep == 2){
    if(controller.YesPressed()){
      WriteMessage("Here it is.");
      delay(outputMessageDelay);
      SubRoutineStep = 3;
    }
    else if(controller.NoPressed()){
      WriteMessage("We will remove\nthat one, then.");
      delay(outputMessageDelay);
      SubRoutineStep = 4;
    }
  }
  else if(SubRoutineStep == 3 || SubRoutineStep == 4){
    int sent_status = 0;
    if(SubRoutineStep == 3){
      sent_status = radio.send(parser.encodeSendColor(color_index));
    }
    else{
      sent_status = radio.send(parser.encodeRemoveColor(color_index));
    }
    if(sent_status == 1  || sent_status == 2){
      Serial.println("Color sent. waiting for message back.");
      int complete_status = 0;
      while(!(radio.receive(complete_status) && complete_status == 1)){
        delay(500);
      }
      Serial.print("Mesage back - ");
      color_index++;
      SubRoutineStep = 1;
      if(color_index == num_painted || sent_status == 2){
        color_index = 0;
        SubRoutineStep = 0;
        artist_status = paint_complete;
      }
    }
  }
}
void PaintCompleteLoop(){
  if(SubRoutineStep == 0){
    WriteMessage("It is beautiful!\nDo you like it?");
    delay(outputMessageDelay);
    SubRoutineStep = 1;
  } 
  else if(SubRoutineStep == 1){
    if(controller.YesPressed() || controller.NoPressed()){
      WriteMessage("Merveilleux!\nIt is fini.");
      delay(outputMessageDelay);
      artist_status = waiting_to_start;
      SubRoutineStep = 0;
      color_index = 0;
      if(controller.NoPressed()){
        int sent_status = 0;
        while(sent_status != 1){
          sent_status = radio.send(parser.encodeStatusChange(reset));
          if(sent_status != 1){
            delay(1000);
          }
        }
      }else{
        sorter_reset = false;
      }
    }
  }
}
void test_loop(){
  color_index = 0;
  num_painted = 0;
  shape_index = 0;
  while(color_index < 12){
    //int sorter_status = radio.send(parser.encode(red[0], green[0], blue[0], colorTone[0], shape_index % 5 + 1));
    int sorter_status = radio.send(parser.encode(red[color_index % numColors], green[color_index % numColors], blue[color_index % numColors], colorTone[color_index % numColors], shape_index % 5 + 1));
    if(sorter_status == reading){
      color_index++;
      num_painted++;
      shape_index++;
      delay(500);
    }else if(sorter_status == ready_to_sort){
      break;
    }
  }
  color_index = 0;
  //sort them!
  while(true){
    if(radio.send(parser.encodeStatusChange(start_sorting)) == sorting){
      break;
    }
    delay(500);
  }
  while(true){
    int value = 0;
    if(radio.receive(value) && value == sort_complete){
      while(!radio.sendResponse(1)){
        delay(500);
      }
      break;
    }
    delay(500);
  }
  
  while(true){
    int sent_status = radio.send(parser.encodeSendColor(color_index));
    if(sent_status == 1  || sent_status == 2){
      int complete_status = 0;
      while(!(radio.receive(complete_status) && complete_status == 1)){
        delay(500);
      }
      
      if(color_index++ == num_painted || sent_status == 2){
        color_index = 0;
        break;
      }
    }
  }
  delay(2000);
  while(radio.send(parser.encodeStatusChange(reset))!= 1){ delay(500);}
  if(digitalRead(DebugTestPin) != HIGH){
    debugging = false;
    controller.FakeInput = false;
  }
}
void loop(void)
{
  if(debugging){
    int idx = color_index % numColors;
    for(int jewel_index = 0; jewel_index < jewel.numPixels(); jewel_index++){
      jewel.setPixelColor(jewel_index,red[idx],green[idx],blue[idx]);    
      delay(20);  
    }
    jewel.show();// Turn it on 
    for(int ring12_index = 0; ring12_index < ring12.numPixels(); ring12_index++){
      ring12.setPixelColor(ring12_index,red[idx],green[idx],blue[idx]);    
      delay(20);  
    }
    ring12.show();// Turn it on
    for(int ring16_index = 0; ring16_index < ring16.numPixels(); ring16_index++){
      ring16.setPixelColor(ring16_index,red[idx],green[idx],blue[idx]);      
    }
    ring16.show();// Turn it on
lcd.setCursor(0,0);
lcd.print(idx);
    delay(1000);
    color_index ++;
    //test_loop();
  }else{
    if(artist_status == waiting_to_start){
      WaitToStartLoop();
    }
    else if(artist_status == choosing_color_and_shape){
      ChooseColorAndShapeLoop();
    }
    else  if(artist_status == waiting_on_sort){
      WaitOnSortLoop();
    }
    else  if(artist_status == painting){
      PaintingLoop();
    }
    else  if(artist_status == paint_complete){
      PaintCompleteLoop();
    }
  }
}
