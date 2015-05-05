#include "Parser.h"

Parser::Parser (){
}
String Parser::encode(byte r, byte g, byte b, byte t, byte s){
  String rtn = "o";
  rtn += r;
  rtn += "-";
  rtn += g;
  rtn += "-";
  rtn += b;
  rtn += "-";
  rtn += t;
  rtn += "-";
  rtn += s;
  rtn += ".";
  return rtn;
}
String Parser::encodeSendColor(byte i){
  String rtn = "s";
  rtn += i;
  rtn += ".";
  return rtn;
}
String Parser::encodeRemoveColor(byte i){
  String rtn = "r";
  rtn += i;
  rtn += ".";
  return rtn;
}
String Parser::encodeStatusChange(byte i){
  String rtn = "c";
  rtn += i;
  rtn += ".";
  return rtn;
}
boolean Parser::decode(char* str, byte len, byte &r, byte &g, byte &b, byte &t, byte &s){
  if(str[0] == 'o'){
    boolean r_complete = false;
    boolean g_complete = false;
    boolean b_complete = false;
    byte val = 0;
    for(byte i = 1;i< len;i++){
      if(str[i] == '-'){
        if(!r_complete){
          r = val;
          r_complete = true;
        }
        else if(r_complete && !g_complete){
          g = val;
          g_complete = true;
        }
        else if(r_complete && g_complete && !b_complete){
          b = val;
          b_complete = true;
        } 
        else {
          t = val;
        }
        val = 0;
      }
      else if(str[i] == '.'){
        s = val;
        return true;
      }
      else{
        val = val*10 + ((byte) str[i]- 48);
      }
    }
  }
  return false;	
}
boolean Parser::decodeMessage(char* str, byte len, byte &t, byte &b){
  if(str[0] == 'r' || str[0] == 's' || str[0] == 'c'){
    if(str[0] == 'r'){
      t = 2;
    }
    else if(str[0] == 's'){
      t = 1;
    }
    else{
      t = 3;
    }
    byte val = 0;
    for(byte i = 1;i< len;i++){
      if(str[i] == '.'){
        b = val;
        return true;
      }
      else{
        val = val*10 + ((byte) str[i]- 48);
      }
    }
  }
  return false;	
}


