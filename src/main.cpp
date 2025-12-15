#include <Arduino.h>
#include "config.h"
#include "interfaces/KeypadInterface.h"


void setup() {
  
  Serial.begin(BAUD_RATE);
  while (!Serial) {
    ;
  }
}

void loop() {
  vTaskDelete(NULL); // everything is handled in tasks, not the loop
}

