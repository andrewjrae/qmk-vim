#pragma once

#include QMK_KEYBOARD_H

typedef bool (*process_func_t)(uint16_t, const keyrecord_t*);

void normal_mode(void);
void insert_mode(void);
void visual_mode(void);
void visual_line_mode(void);
