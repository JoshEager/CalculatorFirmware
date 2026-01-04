/**
 * @file debug.h
 * @author Josh Eager
 * @brief contians definitions for tools for debugging
 * @date 2026-01-03
 * 
 */

#include <Arduino.h>
#include "config.h" 

#ifdef DEBUG
    #define PRINTDBG(x) Serial.print(x)
#else
    #define PRINTDBG(x)
#endif