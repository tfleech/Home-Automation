#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

RF24 radio(9,10);

const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

void setup() {
  Serial.begin(57600);
  Serial.println("Serial Open");
  printf_begin();

  radio.begin();
  radio.setRetries(15,15);

  Serial.println("radio started");

  radio.openWritingPipe(pipes[0]);
  radio.openReadingPipe(1,pipes[1]);

  radio.stopListening();
  radio.printDetails();
  Serial.println("finished Setup");
}

void loop() {
  // Simply forward messages received by Pi via serial connection
  if (Serial.available()){
    int Q = Serial.read();
    RF_Request(Q);
  }
}

void RF_Request(unsigned long query){
  unsigned long time = millis();
  //printf("Now sending %lu...",query);
  bool ok = radio.write( &query, sizeof(unsigned long));

  /*if(ok){
    printf("ok...");
  }else{
    printf("failed.\n\r");
  }*/

  radio.startListening();

  unsigned long started_waiting_at = millis();
  bool timeout = false;
  while(!radio.available() && !timeout){
    if (millis() - started_waiting_at > 200){
      timeout = true;
    }
  }

  if (timeout){
    Serial.println("timeout");
  }else{
    unsigned long response;
    radio.read( &response, sizeof(unsigned long));

    Serial.println(response);
  }

  radio.stopListening();
}

