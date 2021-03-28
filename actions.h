#pragma once

#include "motions.h"

// These are the main keys for each vim core vim action
#define VIM_CHANGE KC_DEL
#define VIM_DELETE VCMD(KC_X) // note that you may prefer a simple delete here since we only are using one clipboard
#define VIM_YANK VCMD(KC_C)
// Other commands
#define VIM_PASTE VCMD(KC_V)
#define VIM_UNDO VCMD(KC_U)
#define VIM_REDO VCMD(KC_Y)
#define VIM_FIND VCMD(KC_F)
#define VIM_SAVE VCMD(KC_S)

// Process function to handle text objects ie in or around word
bool process_text_objects(uint16_t keycode, const keyrecord_t *record);

// The actual change action
void change_action(void);
// The actual delete action
void delete_action(void);
// The yank action for a line
void delete_line_action(void);
// The actual yank action
void yank_action(void);
// The yank action for a line
void yank_line_action(void);
// The paste action
void paste_action(void);
#ifdef VIM_PASTE_BEFORE
// Paste before, ie P
void paste_before_action(void);
#endif

// Visual mode isn't an action, but it does share the same action type sequences,
// so we need a way to make the action not call an old action
void action_prepare_visual(void);

// Function to start a change action
void start_change_action(void);
// Function to start a delete action
void start_delete_action(void);
// Function to start a yank action
void start_yank_action(void);

// Function to start a visual action
// Note that this functions a little differently as there is no action at the end,
// and it doesn't use the process_vim_action function, it basically sets up the global
// variables such that process_text_objects can be used without calling a random action
void start_visual_action(void);
