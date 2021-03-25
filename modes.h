#pragma once

#include QMK_KEYBOARD_H

// Function pointer type for process keycode functions
typedef bool (*process_func_t)(uint16_t, const keyrecord_t*);

// Function to enter into normal mode
void normal_mode(void);
// Function to enter into insert mode
void insert_mode(void);
// Function to enter into visual mode
void visual_mode(void);
// Function to enter into visual line mode
void visual_line_mode(void);
