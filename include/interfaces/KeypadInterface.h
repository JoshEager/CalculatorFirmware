#pragma once

#include <Keypad.h>
#include <Arduino.h>
#include "config.h"

/**
 * @brief Namespace that contains methods for interacting with the keypad. 
 * @author Josh Eager
 * @date 1-4-26
 * 
 * use the KeypadInterface::begin() method to start the keypad driver
 * Use the KeypadInterface::getKey() method to get a key. 
 * 
 */
namespace KeypadInterface
{

    /**
     *
     * @brief A class (with an instance defined below) for interfacing with the keypad
     * @author Josh Eager
     * @date 1-4-26
     *
     * To key keys from the keypad, simply read from keypad->key_queue (defined below) using xQueueRecieve().
     * If a key has been pressed, then it will be placed on the queue. To modify anything
     * about the keypad_interface object itself, check src/interfaces/KeypadInterface.cpp
     * to see how it is initialized. You can change the keymapping there.
     *
     */
    class _KeypadInterface
    {
    private:
        int polling_delay_ms;
        Keypad *kpd;

        TaskHandle_t polling_task_handle;

        void pollingTask();

    public:
        _KeypadInterface(char keys[ROWS][COLS], byte *rowPins, byte *colPins, int polling_delay_ms);

        QueueHandle_t key_queue;

        void begin();
        static void pollingTaskEntry(void *pvParameters);
    };

    extern _KeypadInterface *keypad_interface;


    /**
     * @brief Get any keys off the queue (in keypad_interface->key_queue)
     * @author Josh Eager
     * @date 1-4-25
     * @return char 
     * 
     * If any key has been pressed, it can be gotten by calling this function. If no key 
     * has been pressed, it will return 0 (the NUL character, ascii 0).  
     * May block for up to 5 ms. 
     */
    char getKey();

    /**
     * @brief Starts a task that manages the keypad
     * @author Josh Eager
     * @date 1-4-25
     * 
     */
    void begin();

} // namespace KeypadInterface