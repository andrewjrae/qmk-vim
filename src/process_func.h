#pragma once

#include QMK_KEYBOARD_H

// Function pointer type for process keycode functions
typedef bool (*process_func_t)(uint16_t, const keyrecord_t*);
