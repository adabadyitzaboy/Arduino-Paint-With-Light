#include "Sorter.h"

Sorter::Sorter (NS_Rainbow *ns_strip, byte servoPin, byte redLed, byte greenLed, byte blueLed): 
ns_strip(ns_strip), redLed(redLed), greenLed(greenLed), blueLed(blueLed), hand(servoPin){
  swap_delay = 500;
  swap_complete_delay = 1000;
}
void Sorter::setup(){
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(blueLed, OUTPUT);
  hand.setup();
}
void Sorter::sort(byte num_to_sort){
  hand.attach();
  //PrintValues();  
  while(bubble_sort(num_to_sort)){
    //NOOP
    // Serial.println("round complete");
    // PrintValues();
  }
  // Serial.println("Sort Complete");
  // PrintValues();
  hand.detach();
}
void Sorter::test(){
  hand.test();
  hand.attach();
  byte len = ns_strip->numCells() - 1;
  for(byte idx = 0;idx < len;idx++){
    Serial.print("Moving to ");
    Serial.println(idx+1);
    hand.moveTo(idx);
    delay(400);  
  }  
  hand.moveTo(0);
  delay(swap_delay);  
  hand.detach();
}
boolean Sorter::bubble_sort(byte num_to_sort){
  uint32_t rgb1 = ns_strip->getColor(0);
  boolean swapped = false;
  for(byte idx = 1;idx < num_to_sort;idx++){
    uint32_t rgb2 = ns_strip->getColor(idx);
    if(swap(rgb1, rgb2)){
      swap_colors(idx-1, rgb1, idx, rgb2);
      swapped = true;
    }
    else{
      rgb1 = rgb2;
    }
  }
  return swapped;
}
boolean Sorter::swap(uint32_t rgb1, uint32_t rgb2){
  //true when rgb2 > rgb1
  uint8_t r1 = (uint8_t)(rgb1 >> 16),
  g1 = (uint8_t)(rgb1 >>  8),
  b1 = (uint8_t)rgb1;
  uint8_t r2 = (uint8_t)(rgb2 >> 16),
  g2 = (uint8_t)(rgb2 >>  8),
  b2 = (uint8_t)rgb2;
  byte s1 = get_section(r1, g1, b1);
  byte s2 = get_section(r2, g2, b2);
  if(s1 < s2){
    return false;
  }
  else if(s1 > s2){
    return true;
  }
  else{
    if(s1 == 1){
      return g2 < g1;
    }
    else if(s1 == 2){
      return r2 > r1;
    }
    else if(s1 == 3){
      return b2 > b1;
    }
    else if(s1 == 4){
      return r2 > r1;
    }
    return false;
  }
  return true;
}
byte Sorter::get_section(byte r, byte g, byte b){
  //there are 4 sections for sorting.
  //1 - (255, x, 0) where 0 <= x <=255
  //2 - (x, 255, 0) where 0 <= x <=255
  //3 - (0, 255-x, x) where 0 <= x <=255
  //4 - (0, x, 255) where 0 <= x <=192
  if(r == 255 && b == 0){
    return 1;
  }
  else if(g == 255 && b == 0){
    return 2;
  }
  else if(r == 0 && b == 255){
    return 3;
  }
  else if(g == 0){
    return 4;
  }
  return 5;
}
void Sorter::set_color(byte strip_cell, uint32_t led_rgb, uint32_t strip_rgb){
  hand.moveTo(strip_cell);
  delay(swap_delay);  

  uint8_t r = (uint8_t)(led_rgb >> 16),
  g = (uint8_t)(led_rgb >>  8),
  b = (uint8_t)led_rgb;
  analogWrite(redLed, r);
  analogWrite(greenLed, g);
  analogWrite(blueLed, b);

  ns_strip->setColor(strip_cell, strip_rgb);
  ns_strip->show();
}
void Sorter::swap_colors(byte c1, uint32_t rgb1, byte c2, uint32_t rgb2){
  byte swap_cell = ns_strip->numCells() - 1;

  set_color(c2, rgb2, 0);//grab the color from the spot we are going to move to

  set_color(swap_cell, 0, rgb2);//put the color in the swap spot.

  set_color(c1, rgb1, 0);//grab the color from the spot we are going to move from

  set_color(c2, 0, rgb1);//put the color in the move to spot.

  set_color(swap_cell, rgb2, 0);//grab the color from the swap spot

  set_color(c1, 0, rgb2);//put the color in the move from spot.

  delay(swap_complete_delay);
}
void Sorter::PrintValues(){
  byte len = ns_strip->numCells() - 1;
  for(byte idx = 0;idx < len;idx++){   
    uint32_t rgb = ns_strip->getColor(idx);
    uint8_t r = (uint8_t)(rgb >> 16),
    g = (uint8_t)(rgb >>  8),
    b = (uint8_t)rgb;
    Serial.print(idx);
    Serial.print(" - (");
    Serial.print(r);
    Serial.print(", ");
    Serial.print(g);
    Serial.print(", ");
    Serial.print(b);
    Serial.print(") section - ");
    Serial.print(get_section(r,g,b));
    Serial.println(". ");
  }
}
void Sorter::moveToAndRemove(byte idx, byte &r, byte &g, byte &b){
  if(idx < ns_strip->numCells()){
    hand.attach();
    uint32_t rgb = ns_strip->getColor(idx);
    r = (byte)(rgb >> 16),
    g = (byte)(rgb >>  8),
    b = (byte)rgb;
    hand.moveTo(idx);
    delay(swap_delay);  

    ns_strip->setColor(idx,0);
    ns_strip->show();
    hand.detach();
  }
}



