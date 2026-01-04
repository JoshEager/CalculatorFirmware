#include "interfaces/KeypadInterface.h"

#include <Arduino.h>
#include <Keypad.h>
#include "config.h"
#include "tools/debug.h"

KeypadInterface::KeypadInterface(char keys[ROWS][COLS], byte *rowPins, byte *colPins, int polling_delay_ms)
{
    this->polling_delay_ms = polling_delay_ms;
    kpd = new Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

    // Create a queue for storing the pressed keys
    key_queue = xQueueCreate(KEY_PRESS_QUEUE_LENGTH, sizeof(char));
}

void KeypadInterface::pollingTask()
{
    PRINTDBG("Entered pollingTask()");
    while (1)
    {
        if (kpd->getKey())
        { // getKeys() checks for any key presses, but also populates kpd->key[] if there is any
            for (int i = 0; i < LIST_MAX; i++)
            {
                if (kpd->key[i].kstate == PRESSED)
                {
                    xQueueSend(this->key_queue, &(kpd->key[i].kchar), pdMS_TO_TICKS(KEYPAD_QUEUE_WAIT_TIME_MS));
                    PRINTDBG(String("Key Pressed: ") + kpd->key[i].kchar + '\n');
                }
                else if (kpd->key[i].kstate == HOLD)
                {
                    xQueueSend(this->key_queue, &(kpd->key[i].kchar), pdMS_TO_TICKS(KEYPAD_QUEUE_WAIT_TIME_MS));
                    PRINTDBG(String("Key Held: ") + kpd->key[i].kchar + '\n');
                }
            }
        }
        vTaskDelay(this->polling_delay_ms / portTICK_PERIOD_MS);
    }
}

void KeypadInterface::pollingTaskEntry(void *pvParameters)
{
    KeypadInterface *inst = (KeypadInterface *)pvParameters;
    inst->pollingTask();
    vTaskDelete(NULL);
}

void KeypadInterface::begin()
{
    // Start the polling task. The polling task puts the keys it reads into the key press queue.
    // Note: The task entry function must be a class method and not a normal instance method
    // because of compatibily with the c api for freeRTOS.
    xTaskCreatePinnedToCore(
        pollingTaskEntry,
        "Keypad Driver - polling",
        4096,
        (void *)this,
        1,
        &polling_task_handle, 
        1);
}