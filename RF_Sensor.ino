#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

RF24 radio(9,10);
int lightPin=3;

const int analogIn = A0;
int mVperAmp = 100;
int RawValue = 0;
int ACSoffset = 2500;
double Voltage = 0;
double Amps = 0;

const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

//typedef enum { role_ping_out = 1, role_pong_back } role_e;
//const char* role_friendly_name[] = { "invalid", "Ping out", "Pong back"};
//role_e role = role_pong_back;

//Read value from current sensor
double getSwitchCurrent() {
  RawValue = analogRead(analogIn);
  Voltage = (RawValue / 1024.0)*5000;
  Amps = ((Voltage - ACSoffset) / mVperAmp);
  return Amps;
}

void setup() {
  // put your setup code here, to run once:
  pinMode(4, OUTPUT);
  
  Serial.begin(57600);
  printf_begin();
  printf("\n\rRF24/examples/GettingStarted/\n\r");

  radio.begin();
  radio.setRetries(15,15);

  radio.openWritingPipe(pipes[1]);
  radio.openReadingPipe(1,pipes[0]);

  radio.startListening();

  radio.printDetails();
}

void loop() {
  if (radio.available()){
    Serial.println("Got Something");
    unsigned long query;
    unsigned long resp;
    bool done = false;
    while(!done){
      done = radio.read( &query, sizeof(unsigned long));

      printf("Got query %lu...",query);

      delay(20);
    }

    radio.stopListening();

    //resp = analogRead(4);

    // LightON
    if (query == 49){
      //if (getSwitchCurrent() < 1){
        digitalWrite(4, LOW);
        Serial.println("Turning on light");
      //}
      resp = 1;
      
    // Light OFF
    } else if (query == 50){
      //if (getSwitchCurrent() > 1){
        digitalWrite(4, HIGH);
      //}
      resp = 1;

    // Light Status
    } else if (query == 51){
      if (getSwitchCurrent()>1){
        resp = 2;
      }else{
        resp = 3;
      }
    } else if (query == 52){
      resp = getSwitchCurrent();
    }
    
    radio.write(&resp, sizeof(unsigned long));
    printf("Sent response.\n\r");

    radio.startListening();
  }
}
