#include "modes.h"
#include "numbered_actions.h"
#include "motions.h"
#include "actions.h"
#include "process_func.h"

extern process_func_t process_func;

// forward declare from vim.h
void disable_vim_mode(void);

vim_mode_t vim_current_mode;
vim_mode_t get_vim_mode(void) {
    return vim_current_mode;
}

#ifdef BETTER_VISUAL_MODE
extern visual_direction_t visual_direction;
#endif

#ifdef VIM_G_MOTIONS
static bool process_g_cmd(uint16_t keycode, const keyrecord_t *record) {
    if (record->event.pressed) {
        switch (keycode) {
            case KC_G:
                // this doesn't quite work for all programs
                tap_code16(VCMD(KC_A));
                wait_ms(200);
                tap_code(KC_UP);
                break;
            default:
                break;
        }
        normal_mode();
    }
    return false;
}
#endif

#ifdef VIM_COLON_CMDS
static bool process_colon_cmd(uint16_t keycode, const keyrecord_t *record) {
    if (record->event.pressed) {
        switch (keycode) {
            case KC_Q:
                disable_vim_mode();
                break;
            case KC_W:
                tap_code16(VIM_SAVE);
                break;
            case KC_ENTER:
                normal_mode();
                break;
            default:
                break;
        }
    }
    return false;
}
#endif

// Allow the user to add their own bindings to both normal modes
// Note, this should be optimized away unless there is a user definition
__attribute__ ((weak))
bool process_normal_mode_user(uint16_t keycode, const keyrecord_t *record) {
    return true;
}

// The function that handles normal mode keycode inputs
bool process_normal_mode(uint16_t keycode, const keyrecord_t *record) {
    if (!process_normal_mode_user(keycode, record)) {
        return false;
    }
#ifdef VIM_DOT_REPEAT
    bool should_record_action = true;
    #define NO_RECORD_ACTION() should_record_action = false;
#else
    #define NO_RECORD_ACTION()
#endif

    // process numbers for numbered actions
#ifdef VIM_NUMBERED_JUMPS
    if (!process_numbers(keycode, record)) {
        return false;
    }
#endif
    // handle motions on their own so they can be pressed and held
    if (!process_motions(keycode, record, 0)) {
        return false;
    }
    if (record->event.pressed) {
        switch (keycode) {
            // insert keys
            case LSFT(KC_I):
                VIM_HOME();
                // fallthrough to lowercase i
            case KC_I:
                insert_mode();
                break;
            case LSFT(KC_A):
                VIM_END();
                insert_mode();
                break;
            case KC_A:
                tap_code(KC_RIGHT);
                insert_mode();
                break;
            case LSFT(KC_O):
                VIM_HOME();
                tap_code(KC_ENTER);
                tap_code(KC_UP);
                insert_mode();
                break;
            case KC_O:
                VIM_END();
                tap_code(KC_ENTER);
                insert_mode();
                break;
            // actions
            case LSFT(KC_C):
                VIM_SHIFT_END();
                change_action();
                break;
            case KC_C:
                start_change_action();
                break;
            case LSFT(KC_D):
                VIM_SHIFT_END();
                delete_action();
                break;
            case KC_D:
                start_delete_action();
                break;
            case LSFT(KC_S):
                VIM_HOME();
                VIM_SHIFT_END();
                change_action();
                break;
            case KC_S:
                tap_code16(LSFT(KC_RIGHT));
                change_action();
                break;
            case LSFT(KC_Y):
                VIM_SHIFT_END();
                yank_action();
                NO_RECORD_ACTION();
                break;
            case KC_Y:
                start_yank_action();
                NO_RECORD_ACTION();
                break;
#ifdef VIM_PASTE_BEFORE
            case LSFT(KC_P):
                paste_before_action();
                break;
#endif
            case KC_P:
                paste_action();
                break;
            // visual modes
#ifndef NO_VISUAL_LINE_MODE
            case LSFT(KC_V):
                visual_line_mode();
                NO_RECORD_ACTION();
                break;
#endif
#ifndef NO_VISUAL_MODE
            case KC_V:
                visual_mode();
                NO_RECORD_ACTION();
                break;
#endif
            // undo redo
            case KC_U:
                tap_code16(VIM_UNDO);
                wait_ms(10);
                tap_code(KC_LEFT);
                NO_RECORD_ACTION();
                break;
            case LCTL(KC_R):
                tap_code16(VIM_REDO);
                NO_RECORD_ACTION();
                break;
#ifdef VIM_REPLACE
            case KC_R:
                replace_action();
                break;
#endif
            case KC_X:
                tap_code16(VIM_X);
                break;
            case LSFT(KC_X):
                tap_code16(VIM_SHIFT_X);
                break;
#ifdef VIM_COLON_CMDS
            case KC_COLON:
                process_func = process_colon_cmd;
                NO_RECORD_ACTION();
                break;
#endif
#ifdef VIM_G_MOTIONS
            // g motions
            case LSFT(KC_G):
                // this doesn't quite work for all programs
                tap_code16(VCMD(KC_A));
                wait_ms(200);
                tap_code(KC_DOWN);
                NO_RECORD_ACTION();
                break;
            case KC_G:
                process_func = process_g_cmd;
                NO_RECORD_ACTION();
                break;
#endif
#ifdef VIM_DOT_REPEAT
            case KC_DOT:
                repeat_action(record);
                NO_RECORD_ACTION();
                break;
#endif
            default:
                NO_RECORD_ACTION();
                if (keycode >= QK_MODS && (keycode & 0xFF00) != QK_LSFT) {
                    tap_code16(keycode);
                }
                break;
        }
#ifdef VIM_DOT_REPEAT
        if (should_record_action) {
            start_recording_repeat();
        }
#endif
    }
    return false;
}

// Allow the user to add their own bindings to visual mode
// Note, this should be optimized away unless there is a user definition
__attribute__ ((weak))
bool process_visual_mode_user(uint16_t keycode, const keyrecord_t *record) {
    return true;
}

// The function that handles visual mode keycode inputs
bool process_visual_mode(uint16_t keycode, const keyrecord_t *record) {
    if (!process_visual_mode_user(keycode, record)) {
        return false;
    }
    // process numbers for numbered actions
#ifdef VIM_NUMBERED_JUMPS
    if (!process_numbers(keycode, record)) {
        return false;
    }
#endif
    // handle motions on their own so they can be pressed and held
    if (!process_motions(keycode, record, QK_LSFT)) {
        return false;
    }
#ifdef _VIM_TEXT_OBJECTS
    if (!process_text_objects(keycode, record)) {
        return false;
    }
#endif
    if (record->event.pressed) {
        switch (keycode) {
            case KC_C:
            case KC_S:
                change_action();
                return false;
            case KC_D:
            case KC_X:
                delete_action();
                return false;
            case KC_Y:
                yank_action();
                return false;
            case KC_P:
                tap_code16(VIM_PASTE);
                normal_mode();
                return false;
            case KC_ESC:
#ifdef BETTER_VISUAL_MODE
                if (visual_direction == V_FORWARD)
                    tap_code(KC_RIGHT);
                else
                    tap_code(KC_LEFT);
#else
                tap_code(KC_RIGHT);
#endif
                normal_mode();
                return false;
            default:
                if (keycode >= QK_MODS && (keycode & 0xFF00) != QK_LSFT) {
                    tap_code16(keycode);
                }
                break;
        }
    }
    return false;
}

// Allow the user to add their own bindings to visual line mode
// Note, this should be optimized away unless there is a user definition
__attribute__ ((weak))
bool process_visual_line_mode_user(uint16_t keycode, const keyrecord_t *record) {
    return true;
}

// The function that handles visual line mode keycode inputs
bool process_visual_line_mode(uint16_t keycode, const keyrecord_t *record) {
    if (!process_visual_line_mode_user(keycode, record)) {
        return false;
    }
    // process numbers for numbered actions
#ifdef VIM_NUMBERED_JUMPS
    if (!process_numbers(keycode, record)) {
        return false;
    }
#endif
    // handle motions on their own so they can be pressed and held
    switch (keycode) {
        case KC_J:
        case VIM_J:
#ifdef BETTER_VISUAL_MODE
            if (visual_direction == V_NONE) {
                tap_code(KC_LEFT);
                tap_code16(LSFT(VIM_J));
            }
#endif
            set_visual_direction(V_FORWARD);
            register_motion(LSFT(VIM_J), record);
            return false;
        case KC_K:
        case VIM_K:
            set_visual_direction(V_BACKWARD);
            register_motion(LSFT(VIM_K), record);
            return false;
        }
    if (record->event.pressed) {
        switch (keycode) {
            case KC_C:
            case KC_S:
                tap_code16(LSFT(KC_LEFT));
                change_action();
                return false;
            case KC_D:
            case KC_X:
                delete_line_action();
                return false;
            case KC_Y:
                yank_line_action();
                return false;
            case KC_P:
                tap_code16(VIM_PASTE);
                normal_mode();
                return false;
            case KC_ESC:
#ifdef BETTER_VISUAL_MODE
                if (visual_direction == V_FORWARD)
                    tap_code(KC_RIGHT);
                else
                    tap_code(KC_LEFT);
#else
                tap_code(KC_RIGHT);
#endif
                normal_mode();
                return false;
            default:
                if (keycode >= QK_MODS && (keycode & 0xFF00) != QK_LSFT) {
                    tap_code16(keycode);
                }
                break;
        }
    }
    return false;
}


// Allow the user to add their own bindings to insert mode
// Note, this should be optimized away unless there is a user definition
__attribute__ ((weak))
bool process_insert_mode_user(uint16_t keycode, const keyrecord_t *record) {
    return true;
}

// The function that handles insert mode keycode inputs
bool process_insert_mode(uint16_t keycode, const keyrecord_t *record) {
    if (!process_insert_mode_user(keycode, record)) {
        return false;
    }
    // handle motions on their own so they can be pressed and held
    if (record->event.pressed) {
        switch (keycode) {
            case KC_ESC:
                normal_mode();
                return false;
            default:
                break;
        }
    }
    return true;
}


// Allow the user to set custom state when normal mode is entered
__attribute__ ((weak))
void normal_mode_user(void) {
}

// Function to enter into normal mode
void normal_mode(void) {
    normal_mode_user();
    vim_current_mode = NORMAL_MODE;
    process_func = process_normal_mode;
}

// Allow the user to set custom state when insert mode is entered
__attribute__ ((weak))
void insert_mode_user(void) {
}

// Function to enter into insert mode
void insert_mode(void) {
    insert_mode_user();
    vim_current_mode = INSERT_MODE;
    // need to clear motion keys if they are currently pressed
    clear_keyboard();
    process_func = process_insert_mode;
}

// Allow the user to set custom state when visual mode is entered
__attribute__ ((weak))
void visual_mode_user(void) {
}

// Function to enter into visual mode
void visual_mode(void) {
    visual_mode_user();
    vim_current_mode = VISUAL_MODE;
#ifndef NO_VISUAL_MODE
#ifdef BETTER_VISUAL_MODE
    visual_direction = V_NONE;
#endif
    // this sets up the actions states so we can use text objects
    start_visual_action();
    process_func = process_visual_mode;
#endif
}


// Allow the user to set custom state when visual line mode is entered
__attribute__ ((weak))
void visual_line_mode_user(void) {
}

// Function to enter into visual line mode
void visual_line_mode(void) {
    visual_line_mode_user();
    vim_current_mode = VISUAL_LINE_MODE;
#ifndef NO_VISUAL_LINE_MODE
#ifdef BETTER_VISUAL_MODE
    visual_direction = V_NONE;
#endif
    VIM_END();
    tap_code(KC_RIGHT);
    tap_code16(LSFT(KC_UP));

    process_func = process_visual_line_mode;
#endif
}
