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

// Keypad settings
#define MAX_ROWS 7
#define MAX_COLS 4
