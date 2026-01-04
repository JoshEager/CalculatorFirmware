#pragma once

#include <Keypad.h>
#include <Arduino.h>
#include <array>
#include "config.h"

/**
 * 
 * @brief A class (with an instance defined below) for interfacing with the keypad 
 * @author Josh Eager
 * @date 1-4-26
 * 
 * To key keys from the keypad, simply read from keypad->key_queue using xQueueRecieve().
 * If a key has been pressed, then it will be placed on the queue. To modify anything 
 * about the keypad_interface object itself, check src/interfaces/KeypadInterface.cpp
 * to see how it is initialized. You can change the keymapping there.
 * 
 */

class KeypadInterface 
{
private:
    int polling_delay_ms;
    Keypad* kpd;

    TaskHandle_t polling_task_handle;
    

    void pollingTask();

public:
    KeypadInterface(char keys[ROWS][COLS], byte *rowPins, byte *colPins, int polling_delay_ms);

    QueueHandle_t key_queue;

    void begin();
    static void pollingTaskEntry(void *pvParameters);
};

extern KeypadInterface *keypad_interface;