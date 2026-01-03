#include <Arduino.h>
#include "config.h"
#include "interfaces/KeypadInterface.h"

// Keypad interface initialization of static objects
KeypadInterface *keypad_interface;
char key_mappings[ROWS][COLS] = {
    {'a', 'b', 'c', 'd'},
    {'e', 'f', 'g', 'h'},
    {'i', 'j', 'k', 'l'},
    {'m', 'n', 'o', 'p'},
    {'q', 'r', 's', 't'},
    {'u', 'v', 'w', 'x'},
    {'y', 'z', '0', '1'},
};
byte rowPins[] = {ROW1_PIN, ROW2_PIN, ROW3_PIN, ROW4_PIN, ROW5_PIN, ROW6_PIN, ROW7_PIN};
byte colPins[] = {COL1_PIN, COL2_PIN, COL3_PIN, COL4_PIN};

void setup()
{

  Serial.begin(BAUD_RATE);
  while (!Serial)
  {
    ;
  }

  keypad_interface = new KeypadInterface(key_mappings, rowPins, colPins, 50);
  keypad_interface->begin();
}

void loop()
{
  // Everything handeld in tasks
  vTaskDelete(NULL);
}
