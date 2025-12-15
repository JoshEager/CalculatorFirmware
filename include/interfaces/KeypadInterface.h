#pragma once

#include <Keypad.h>
#include <Arduino.h>
#include "config.h"

class KeypadInterface
{
private:
    // TODO: I thought that you couldn't have const member variables because consts have to
    // be initialized and assigned on the same line (and you have to assign them in a constructor),
    // but I was wrong because the constuctor can use a member initializer list (which mine don't)
    int rows;
    int columns;
    int polling_rate_ms;
    char key_array[MAX_ROWS][MAX_COLS];
    byte rowPins[MAX_ROWS];
    byte colPins[MAX_COLS];
    Keypad* kpd;

    TaskHandle_t polling_task_handle;
    QueueHandle_t key_queue;

    void pollingTask();

public:
    KeypadInterface(int rows, int columns, int polling_rate_ms, char **key_array, byte *rowPins, byte *colPins);

    void begin();
    static void pollingTaskEntry(void *pvParameters);
};