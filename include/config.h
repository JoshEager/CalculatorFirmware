#pragma once

/**
 * @file config.h
 * @brief Contains config information for the calculator
 *
 * All the config information is done with macros so that it can be done at compile time.
 *
 * @author Josh Eager
 * @date 2025-12-14
 */

#include "pinout.h"

#define BAUD_RATE 115200
#define DEBUG

// Keypad settings
#define ROWS 7
#define COLS 4
#define KEY_PRESS_QUEUE_LENGTH 10 // characters/keys
#define KEYPAD_QUEUE_WAIT_TIME_MS 50