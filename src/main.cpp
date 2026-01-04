#include <Arduino.h>
#include "config.h" // for the baud rate of serial
#include "interfaces/KeypadInterface.h"

void setup()
{
  Serial.begin(BAUD_RATE);
  while (!Serial)
  {
    ;
  }

  keypad_interface->begin();
}

void loop()
{
  // Everything handeld in tasks
  vTaskDelete(NULL);
}
