#include "interfaces/KeypadInterface.h"

#include <Arduino.h>
#include <Keypad.h>
#include <stdexcept>
#include "config.h"

/**
 * @brief Construct a new Keypad Interface:: Keypad Interface object
 *
 * The keypad interface is what the rest of the application should use
 * to interact with the keypad driver. It provides a convenient way to
 * get key presses from the keypad as well as start, stop, and configure
 * the keypad driver. This interface is meant to work with the Keypad
 * arduino library.
 *
 * Whenever a key is pressed, the key is added to a freeRTOS queue. If
 * anything would like to get key presses, they they simply need to read
 * off the queue.
 *
 * Because of the way queues work, however, only 1 aspect of the applicaion
 * should access the key press queue at a time because once anything reads
 * off the the queue, it removes it for everything else. This means that
 * only the shell should read key presses from the queue and then pass them
 * to the functions that need them, otherwise the specific function or
 * subapplication that the keypress is meant for may not actually receive the
 * key press.
 *
 * @author Josh Eager
 * @date 2025-12-14
 *
 * @param rows The number of rows on the keypad
 * @param columns The number of columns on the keypad
 * @param polling_rate_ms The delay for the polling thread that gets the keys (maybe more accurate to call this polling delay)
 * @param key_array A 2 dimensional array that contains the locations of each key. Must be rows x columns.
 * @param rowPins An array containing the pins for each row (should be of size rows)
 * @param colPins An array containing the pins for each column (should be of size columns)
 *
 */
KeypadInterface::KeypadInterface(int rows, int columns, int polling_rate_ms, char **key_array, byte *rowPins, byte *colPins)
{
    if (rows > MAX_ROWS)
    {
        throw std::out_of_range("Rows must be less than MAX_ROWS");
    }
    if (columns > MAX_COLS)
    {
        throw std::out_of_range("Columns must be less than MAX_COLS");
    }
    this->rows = rows;
    this->columns = columns;
    this->polling_rate_ms = polling_rate_ms;

    // Since the arrays given are likely stack pointers, we should copy their contents to their respective instance property
    // It's also worth noting that it's hard to confirm the user passed an array with the right dimensions.
    // TODO: find memory safety for the afforementioned issue
    for (int i = 0; i < rows; i++)
    {
        this->rowPins[i] = rowPins[i];
        for (int j = 0; j < columns; j++)
        {
            this->key_array[i][j] = key_array[i][j];
            this->colPins[j] = colPins[j];
        }
    }

    *kpd = Keypad(makeKeymap(this->key_array), this->rowPins, this->colPins, this->rows, this->columns);
}

void KeypadInterface::pollingTask()
{
    while (1) {
        

        vTaskDelay(polling_rate_ms);
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
    xTaskCreate(
        pollingTaskEntry,
        "Keypad Driver - polling",
        2048,
        (void *)this,
        1,
        &polling_task_handle);
}