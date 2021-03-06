//Euton/Rowe Project 3c - Matrix Model
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "Radio.h"
#include "Parser.h"
#include <Adafruit_GFX.h>   // Core graphics library
#include <RGBmatrixPanel.h> // Hardware-specific library

//define pins
#define CLK 11  // MUST be on PORTB! (Use pin 11 on Mega)
#define OE  9
#define LAT 10
#define A   2
#define B   3
#define C   4
#define D   5
#define WIFI_CEPin 12
#define WIFI_CSNPin 13
#define DebugTestPin A4
#define MAX_SHAPES 11
#define RadioRole 3

//define enumerations
typedef enum { line = 1, circle = 2, rectangle = 3, round_rectangle = 4, triangle = 5 } shapes_e;

//Global variables
byte shapes[MAX_SHAPES];
uint16_t colors[MAX_SHAPES];
byte paint_index = 0;
boolean debugging = false;

//Create objects
RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE, false);
Radio radio(RadioRole, WIFI_CEPin,WIFI_CSNPin);//controls reading/writing to other modules.
Parser parser = Parser();//encodes and decodes the values.

void setup() {
  
  Serial.begin(57600);
  Serial.print("Starting: ");
  
  radio.setup();
  
  matrix.begin();  
  matrix.fillScreen(0);//fill with black
  Serial.println("Setup Complete");
  randomSeed(42);
  
  pinMode(DebugTestPin, INPUT);
  if(digitalRead(DebugTestPin) == HIGH){
    debugging = true;
    Serial.println("debugging");
  }
}

void drawShapes(){
  matrix.fillScreen(0);//fill with black
  matrix.drawRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));
  delay(200);
  int shapes_per_row = 5;
  int num_rows = 1;
  if(paint_index < shapes_per_row){
    shapes_per_row = paint_index + 1;
  }
  if(paint_index > shapes_per_row){
    num_rows = paint_index / shapes_per_row;
    if(paint_index % shapes_per_row > 0){
      num_rows++;
    }
  }
  int shape_width = (32 - 1 * (shapes_per_row + 1)) / shapes_per_row; 
  int shape_height= (32 - 1 * (num_rows + 1)) / num_rows; 
  int half_width = shape_width / 2;
  int half_height = shape_height / 2;
  int radius = half_width;
  if(radius > half_height){
    radius = half_height;
  }
//  Serial.print("spr-");
//  Serial.print(shapes_per_row);
//  Serial.print("ns-");
//  Serial.print(num_rows);
//  Serial.print("sw-");
//  Serial.print(shape_width);
//  Serial.print("sh-");
//  Serial.print(shape_height);
//  Serial.print("hw-");
//  Serial.print(half_width);
//  Serial.print("hh-");
//  Serial.print(half_height);
//  Serial.print("r-");
//  Serial.println(radius);
  for(byte idx = 0;idx < paint_index;idx++){
    int column = idx % shapes_per_row;
//    int row = idx % num_rows;
    int x1 = shape_width  * column  + 1;
    int y1 = shape_height * (idx / shapes_per_row) + 1;
    if(column > 0){
      x1 += column -1;
    }
    int x2 = x1 + shape_width - 1;
    int y2 = y1 + shape_height - 1;
//    Serial.print("i-");
//    Serial.print(idx);
//    Serial.print("r-");
//    Serial.print(idx / shapes_per_row);
//    Serial.print("c-");
//    Serial.print(column);
//    Serial.print("x-");
//    Serial.print(x1);
//    Serial.print("y-");
//    Serial.print(y1);
//    Serial.print("s-");
//    Serial.println(shapes[idx]);
    if(shapes[idx] == line){
      matrix.drawLine(x1, y1 + half_height, x2, y1 + half_height, colors[idx]);
    }else if(shapes[idx] == circle){
      matrix.drawCircle(x1 + half_width, y1 + half_height, radius, colors[idx]);
    }else if(shapes[idx] == rectangle){
      matrix.drawRect(x1, y1, shape_width, shape_height, colors[idx]);
    }else if(shapes[idx] == round_rectangle){
      matrix.drawRoundRect(x1, y1, shape_width, shape_height, radius, colors[idx]);
    }else if(shapes[idx] == triangle){
      matrix.drawTriangle(x1, y2, x1 + half_width, y1, x2, y2, colors[idx]);
    }
  }
  Serial.println("");
}
void drawShape(uint16_t color, byte shape){
  byte shape_width = random(3, 30); 
  byte shape_height= random(3, 30); 
  byte half_width = shape_width / 2;
  byte half_height = shape_height / 2;
  byte radius = half_width;
  if(random(0,10) > 5){
    radius = half_height;
  }
  byte x1 = random(1,30 - shape_width);
  byte y1 = random(1,30 - shape_height);
  byte x2 = x1 + shape_width;
  byte y2 = y1 + shape_height;
  Serial.print("Color - ");
  Serial.println(color);
  if(random(0,10) > 5){
    if(shape == line){
      matrix.drawLine(x1, y1 + half_height, x2, y1 + half_height, color);
    }else if(shape == circle){
      matrix.drawCircle(x1 + half_width, y1 + half_height, radius, color);
    }else if(shape == rectangle){
      matrix.drawRect(x1, y1, shape_width, shape_height, color);
    }else if(shape == round_rectangle){
      matrix.drawRoundRect(x1, y1, shape_width, shape_height, radius, color);
    }else if(shape == triangle){
      matrix.drawTriangle(x1, y2, x1 + half_width, y1, x2, y2, color);
    }
  }else{
    if(shape == line){
      matrix.drawLine(x1, y1 + half_height, x2, y1 + half_height, color);
    }else if(shape == circle){
      matrix.fillCircle(x1 + half_width, y1 + half_height, radius, color);
    }else if(shape == rectangle){
      matrix.fillRect(x1, y1, shape_width, shape_height, color);
    }else if(shape == round_rectangle){
      matrix.fillRoundRect(x1, y1, shape_width, shape_height, radius, color);
    }else if(shape == triangle){
      matrix.fillTriangle(x1, y2, x1 + half_width, y1, x2, y2, color);
    }
  }
}
uint16_t convertColor(byte r, byte g, byte b){
  int r1 = map(r, 0, 255, 0, 7);
  int g1 = map(g, 0, 255, 0, 7);
  int b1 = map(b, 0, 255, 0, 7);
  Serial.print("r-");
  Serial.print(r1);
  Serial.print("g-");
  Serial.print(g1);
  Serial.print("b-");
  Serial.println(b1);
  return matrix.Color333(r1, g1, b1);
}
void radio_loop(void)
{
  byte len = 0;
  char* str = radio.receive(len);
  if(len > 0 && len < 11){
    byte message = 0;
    byte messageType = 0;
    if(parser.decodeMessage(str, len, messageType, message) && messageType == 3 && message == 7){   
      while(!radio.send(1)){//done
        delay(500);
      }
      matrix.fillScreen(0);         
    }else{
      Serial.print("Failed read\r\n");
      Serial.print(str);
      Serial.print(" - ");
      Serial.println(len);
    }
  }else if(len > 0){
    //output the color
    byte rgb[3];
    byte color_tone;
    byte shape;
    if(parser.decode(str, len, rgb[0], rgb[1], rgb[2], color_tone, shape)){
      while(!radio.sendResponse(42)){
        delay(500);
      }
      Serial.print("read");
      Serial.println(str);
      drawShape(convertColor(rgb[0], rgb[1], rgb[2]), shape);
    }else{
      Serial.print("Failed read\r\n");
      Serial.print(str);
      Serial.print(" - ");
      Serial.println(len);
    }
  }
}
void test_loop(){
  matrix.drawPixel(0, 0, matrix.Color333(7, 7, 7)); 
  delay(500);

  // fix the screen with green
  matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 7, 0));
  delay(500);

  // draw a box in yellow
  matrix.drawRect(0, 0, 32, 32, matrix.Color333(7, 7, 0));
  delay(500);
  
  // draw an 'X' in red
  matrix.drawLine(0, 0, 31, 31, matrix.Color333(7, 0, 0));
  matrix.drawLine(31, 0, 0, 31, matrix.Color333(7, 0, 0));
  delay(500);
  
  // draw a blue circle
  matrix.drawCircle(10, 10, 10, matrix.Color333(0, 0, 7));
  delay(500);
  
  // fill a violet circle
  matrix.fillCircle(21, 21, 10, matrix.Color333(7, 0, 7));
  delay(500);
  
  // fill the screen with 'black'
  matrix.fillScreen(matrix.Color333(0, 0, 0));
  
  // draw some text!
  matrix.setCursor(1, 0);    // start at top left, with one pixel of spacing
  matrix.setTextSize(1);     // size 1 == 8 pixels high
  matrix.setTextWrap(false); // Don't wrap at end of line - will do ourselves

  matrix.setTextColor(matrix.Color333(7,7,7));
  matrix.println(" Ada");
  matrix.println("fruit");
  
  // print each letter with a rainbow color
  matrix.setTextColor(matrix.Color333(7,0,0));
  matrix.print('3');
  matrix.setTextColor(matrix.Color333(7,4,0)); 
  matrix.print('2');
  matrix.setTextColor(matrix.Color333(7,7,0));
  matrix.print('x');
  matrix.setTextColor(matrix.Color333(4,7,0)); 
  matrix.print('3');
  matrix.setTextColor(matrix.Color333(0,7,0));  
  matrix.println('2');
  
  matrix.setTextColor(matrix.Color333(0,7,7)); 
  matrix.print('*');
  matrix.setTextColor(matrix.Color333(0,4,7)); 
  matrix.print('R');
  matrix.setTextColor(matrix.Color333(0,0,7));
  matrix.print('G');
  matrix.setTextColor(matrix.Color333(4,0,7)); 
  matrix.print("B");
  matrix.setTextColor(matrix.Color333(7,0,4)); 
  matrix.print("*");
  delay(2000);
  if(digitalRead(DebugTestPin) == HIGH){
    debugging = true;
  }else{
    matrix.fillScreen(0);
    debugging = false;
  }
}
void loop(void)
{
  if(debugging){
    test_loop();
  }else{
    radio_loop();
  }  
}
