#pragma once

#include QMK_KEYBOARD_H

// Define a custom variable for the common motion shortcut modifier
// ie on MAC, OPT + RIGHT is forward word, but on Windows/Linux it's CTRL + RIGHT
// This should be used whenever using one of these shortcuts
#ifdef VIM_FOR_MAC
#define VMOTION LOPT

#define VIM_0 LCMD(KC_LEFT)
#define VIM_HOME() tap_code16(VIM_0);
#define VIM_SHIFT_HOME() tap_code16(LSFT(VIM_0));

#define VIM_DLR LCMD(KC_RIGHT)
#define VIM_END() tap_code16(VIM_DLR);
#define VIM_SHIFT_END() tap_code16(LSFT(VIM_DLR));

#else // Windows / Linux

#define VMOTION LCTL

#define VIM_0 KC_HOME
#define VIM_HOME() tap_code(VIM_0);
#define VIM_SHIFT_HOME() tap_code16(LSFT(VIM_0));

#define VIM_DLR KC_END
#define VIM_END() tap_code(VIM_DLR);
#define VIM_SHIFT_END() tap_code16(LSFT(VIM_DLR));

#endif

// The vim motions keys supported by single keystrokes/chords
#define VIM_H KC_LEFT
#define VIM_J KC_DOWN
#define VIM_K KC_UP
#define VIM_L KC_RIGHT

#define VIM_B VMOTION(KC_LEFT)
#define VIM_W VMOTION(KC_RIGHT)
#define VIM_E VMOTION(KC_RIGHT)

// An enum for the direction of the visual mode
// This is used to figure out which way to exit out of a selection
// Note that it's only ever used when BETTER_VISUAL_MODE is defined
// Also note that this will only garner expected behaviour if the
// the user doesn't double back on their original cursor position
typedef enum {
    V_NONE = 0,
    V_FORWARD,
    V_BACKWARD,
} visual_direction_t;

// Function to set the global visual direction if it is currently unset
void set_visual_direction(visual_direction_t dir);

// Register one of the basic vim motions in a fashion where they can be held down.
void register_motion(uint16_t keycode, const keyrecord_t *record);
bool process_motions(uint16_t keycode, const keyrecord_t *record, uint16_t qk_mods);
