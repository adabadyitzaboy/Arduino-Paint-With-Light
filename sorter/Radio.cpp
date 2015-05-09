#include "Radio.h"

Radio::Radio(int role, int pin1, int pin2):
radio(pin1, pin2), role(role)
{
  pipes[0] = 0xF0F0F0F0E1LL;
  pipes[1] = 0xF0F0F0F0D2LL;
  pipes[2] = 0xF0F0F0F0E3LL;
  pipes[3] = 0xF0F0F0F0E4LL;
}
void Radio::setup(){
  //
  // Setup and configure rf radio
  //
  radio.begin();

  // optionally, increase the delay between retries & # of retries
  radio.setRetries(15,15);

  // optionally, reduce the payload size.  seems to
  // improve reliability
  radio.setPayloadSize(payload_size);

  if(role == 1){
    radio.openWritingPipe(pipes[0]);
    radio.openReadingPipe(1,pipes[1]);
  }
  else if(role == 2){
    radio.openWritingPipe(pipes[1]);
    radio.openReadingPipe(1,pipes[0]);
    radio.openReadingPipe(2,pipes[2]);
  }
  else if(role == 3){
    radio.openWritingPipe(pipes[2]);
    radio.openReadingPipe(1,pipes[3]);	
  }
  else {
    Serial.println("Unknown role");
  }
  radio.startListening();

  //radio.printDetails();
}

void Radio::changeWritingChannel(int channel){
  if(role == 2){
    if(channel == 1){
      radio.openWritingPipe(pipes[1]);
    }
    else{
      radio.openWritingPipe(pipes[3]);
    }	
  }
}
int Radio::send(String str){
  Serial.print("Sending string");
  Serial.print(str);
  int rtn = 0;

  radio.stopListening();// First, stop listening so we can talk.
  int messageSize = str.length();
  bool ok = radio.write(str.c_str(),messageSize);
  unsigned long sent_time = millis();

  // Now, continue listening
  radio.startListening();

  if (ok)
    Serial.print("ok...");
  else{
    Serial.print("failed.\n\r");
    return rtn;
  }

  // Wait here until we get a response, or timeout (500ms)
  unsigned long started_waiting_at = millis();
  bool timeout = false;

  while ( ! radio.available() && ! timeout )
    if (millis() - started_waiting_at > 500 )
      timeout = true;

  // Describe the results
  if ( timeout )
  {
    Serial.print("Failed, response timed out.\n\r");
    delay(1000);//Try again 1s later
  }
  else
  {
    // Grab the response, compare, and send to debugging spew
    radio.read( &rtn, sizeof(int) );

    // Spew it
    Serial.print("Got response ");
    Serial.print(rtn);
    Serial.print(", round-trip delay: ");
    Serial.print(millis()-sent_time);
    Serial.print("\n\r");
  }
  return rtn;
}

boolean Radio::send(int status){
  Serial.print("Sending status");
  Serial.print(status);

  radio.stopListening();

  boolean ok = radio.write( &status, sizeof(int) );
  radio.startListening();	

  if (ok)
    Serial.print("ok...");
  else{
    Serial.println("failed.");
    return false;
  }


  unsigned long started_waiting_at = millis();
  bool timeout = false;

  while ( ! radio.available() && ! timeout )
    if (millis() - started_waiting_at > 5000 )
      timeout = true;

  // Describe the results
  if ( timeout )
  {
    Serial.println("Failed, response timed out.\n\r");
  }
  else
  {
    Serial.println("");
    boolean received;
    // Grab the response, compare, and send to debugging spew
    radio.read( &received, sizeof(boolean) );      
    return true;
  }
  return false;
}
boolean Radio::receive(int &value){
  boolean read = radio.read( &value, sizeof(int) );
  if(read){
    boolean received = true;
    radio.stopListening();
    boolean wrote = radio.write( &received, sizeof(boolean) );
    radio.startListening();
    return wrote;
  }
  return false;
}
char* Radio::receive(uint8_t &len){
  len = 0;
  // if there is data ready
  if ( radio.available() )
  {
    // Dump the payloads until we've gotten everything
    int val;
    boolean done = false;
    boolean success = true;
    // Dump the payloads until we've gotten everything
    len = radio.getDynamicPayloadSize();
    done = radio.read( receive_payload, len );
    
    for(int idx = 0;idx < payload_size;idx++){
      if((int)receive_payload[idx] == 0){
        len = idx;
        break;
      }
    }
    
    return receive_payload;
  }
  return "";
}

boolean Radio::sendResponse(int status){
  // First, stop listening so we can talk
  radio.stopListening();

  // Send the final one back.
  boolean success = radio.write( &status, sizeof(int) );

  radio.startListening();

  return success;
}



