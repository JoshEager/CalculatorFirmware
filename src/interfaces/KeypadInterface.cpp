#include "interfaces/KeypadInterface.h"

#include <Arduino.h>
#include <Keypad.h>
#include "config.h"
#include "tools/debug.h"

namespace KeypadInterface
{

    _KeypadInterface::_KeypadInterface(char keys[ROWS][COLS], byte *rowPins, byte *colPins, int polling_delay_ms)
    {
        this->polling_delay_ms = polling_delay_ms;
        kpd = new Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

        // Create a queue for storing the pressed keys
        key_queue = xQueueCreate(KEY_PRESS_QUEUE_LENGTH, sizeof(char));
    }

    void _KeypadInterface::pollingTask()
    {
        PRINTDBG("Entered pollingTask()\n");
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

    void _KeypadInterface::pollingTaskEntry(void *pvParameters)
    {
        _KeypadInterface *inst = (_KeypadInterface *)pvParameters;
        inst->pollingTask();
        vTaskDelete(NULL);
    }

    void _KeypadInterface::begin()
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

    static char key_mappings[ROWS][COLS] = {
        {'a', 'b', 'c', 'd'},
        {'e', 'f', 'g', 'h'},
        {'i', 'j', 'k', 'l'},
        {'m', 'n', 'o', 'p'},
        {'q', 'r', 's', 't'},
        {'u', 'v', 'w', 'x'},
        {'y', 'z', '0', '1'},
    };
    static byte rowPins[] = {ROW1_PIN, ROW2_PIN, ROW3_PIN, ROW4_PIN, ROW5_PIN, ROW6_PIN, ROW7_PIN};
    static byte colPins[] = {COL1_PIN, COL2_PIN, COL3_PIN, COL4_PIN};

    _KeypadInterface *keypad_interface = new _KeypadInterface(key_mappings, rowPins, colPins, 50);



    char getKey()
    {
        static char key;

        if (xQueueReceive(keypad_interface->key_queue, &key, 5 / portTICK_PERIOD_MS) == pdTRUE)
        {
            return key;
        }
        else
        {
            return 0; // NUL character, meaning that no character was on the queue
        }
    }

    void begin() {
        keypad_interface->begin();
    }

} // namespace KeypadInterface