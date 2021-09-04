#pragma once

#include QMK_KEYBOARD_H

// Process hook for numbered action motion counter
bool process_numbers(uint16_t keycode, const keyrecord_t *record);

// Function to safely decrement the motion counter
void decrement_motion_counter(void);

#ifdef VIM_NUMBERED_JUMPS

// Define the do numbered action command which will repeat the given code
// according to the motion counter
#define DO_NUMBERED_ACTION(ACTION) \
    do { \
        extern int16_t motion_counter; \
        do { \
            ACTION; \
        } while (motion_counter && motion_counter-- > 1); \
    } while (0)

#else
#define DO_NUMBERED_ACTION(ACTION) \
    do { \
        ACTION; \
    } while (0)
#endif

    
