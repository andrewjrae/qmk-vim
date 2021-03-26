#pragma once

#include "motions.h"

// These are the main keys for each vim core vim action
#define VIM_CHANGE KC_DEL
#define VIM_DELETE VCMD(KC_X) // note that you may prefer a simple delete here since we only are using one clipboard
#define VIM_YANK VCMD(KC_C)
#define VIM_PASTE VCMD(KC_V)

bool process_text_objects(uint16_t keycode, const keyrecord_t *record);

// The actual change action
void change_action(void);
// The actual delete action
void delete_action(void);
// The actual yank action
void yank_action(void);

// Visual mode isn't an action, but it does share the same action type sequences,
// so we need a way to make the action not call an old action
void action_prepare_visual(void);

// Function to start a change action
void start_change_action(void);
// Function to start a delete action
void start_delete_action(void);
// Function to start a yank action
void start_yank_action(void);
