#pragma once

#include QMK_KEYBOARD_H

int16_t motion_counter = 0;
bool process_numbers(uint16_t keycode, const keyrecord_t *record) {
#ifdef VIM_NUMBERED_JUMPS
    if (keycode >= KC_1 && keycode <= KC_0) {
        if (record->event.pressed) {
            motion_counter *= 10;
            if (keycode != KC_0) {
                motion_counter += keycode - KC_1 + 1;
            }
        }
        return motion_counter > 0;
    }
#endif
    return false;
}

// Define the do numbered action command which will repeat the given code
// according to the motion counter
#ifdef VIM_NUMBERED_JUMPS
#define DO_NUMBERED_ACTION(ACTION) \
    do { \
        ACTION \
    } while (motion_counter && motion_counter-- > 1)
#else
#define DO_NUMBERED_ACTION(ACTION) \
    do { \
        ACTION \
    } while (0)
#endif

    
