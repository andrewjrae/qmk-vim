#pragma once

#include QMK_KEYBOARD_H

// this should cover most of the mac shortcuts
#ifdef VIM_FOR_MAC
#define VCMD LCMD
#else
#define VCMD LCTL
#endif

#define VIM_H KC_LEFT
#define VIM_J KC_DOWN
#define VIM_K KC_UP
#define VIM_L KC_RIGHT

#define VIM_B VCMD(KC_LEFT)
#define VIM_W VCMD(KC_RIGHT)

#define VIM_0 KC_HOME
#define VIM_DLR KC_END

typedef enum {
    V_NONE = 0,
    V_FORWARD,
    V_BACKWARD,
} visual_direction_t;

void set_visual_direction(visual_direction_t dir);

void register_motion(uint16_t keycode, const keyrecord_t *record);
bool process_motions(uint16_t keycode, const keyrecord_t *record, uint16_t qk_mods);
