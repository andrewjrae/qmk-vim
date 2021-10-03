#include "actions.h"
#include "numbered_actions.h"
#include "motions.h"
#include "process_func.h"


// Function pointer type for executing actions
typedef void (*action_func_t)(void);

// the current process func, from in process_func.c
extern process_func_t process_func;

// forward declare insert_mode
void normal_mode(void);
void insert_mode(void);
bool process_normal_mode(uint16_t keycode, const keyrecord_t *record);

// which key is the current action key
static uint16_t action_key;
// the current action_function
static action_func_t action_func;
// whether the current yank is a line or not
static bool yanked_line;

// Function to process key codes when in an action sequence
static bool process_vim_action(uint16_t keycode, const keyrecord_t *record) {
    // process numbers for numbered actions
#ifdef VIM_NUMBERED_JUMPS
    if (!process_numbers(keycode, record)) {
        return false;
    }
#endif
    // process motion and exit call action func if there was one
    if (!process_motions(keycode, record, QK_LSFT)) {
        clear_keyboard();
        action_func();
        return false;
    }

#ifdef _VIM_TEXT_OBJECTS
    if (!process_text_objects(keycode, record)) {
        return false;
    }
#endif

    // handle double taps, ie cc, yy, dd
    if (record->event.pressed) {
        if (keycode == action_key) {
            // for the change action
            if (action_func == change_action) {
                VIM_HOME();
                VIM_SHIFT_END();
#ifdef VIM_NUMBERED_JUMPS
                extern int16_t motion_counter;
                if (motion_counter > 0) {
                    tap_code16(LSFT(KC_RIGHT));
                    decrement_motion_counter();
                    DO_NUMBERED_ACTION(
                        tap_code16(LSFT(KC_DOWN));
                    );
                }
#endif
                action_func();
            }
            else {
                VIM_END();
                tap_code(KC_RIGHT);
                tap_code(KC_UP);
                DO_NUMBERED_ACTION(
                    tap_code16(LSFT(KC_DOWN));
                );
                action_func();
                yanked_line = true;
            }
            return false;
        }
        // if nothing happened, return to normal mode
        normal_mode();
    }

    return false;
}

#ifdef VIM_I_TEXT_OBJECTS
static bool process_in_object(uint16_t keycode, const keyrecord_t *record) {
    if (record->event.pressed) {
        switch (keycode) {
            case KC_W:
                tap_code16(VIM_W);
                tap_code16(LSFT(VIM_B));
                action_func();
                return false;
            case KC_G:
                tap_code16(VCMD(KC_A));
                action_func();
                return false;
            default:
                normal_mode();
                return false;
        }
    }
    return false;
}
#endif

#ifdef VIM_A_TEXT_OBJECTS
static bool process_around_object(uint16_t keycode, const keyrecord_t *record) {
    if (record->event.pressed) {
        switch (keycode) {
            case KC_W:
                tap_code16(VIM_W);
                tap_code16(VIM_W);
                tap_code16(VIM_B);
                tap_code16(LSFT(VIM_B));
                action_func();
                return false;
            case KC_G:
                tap_code16(VCMD(KC_A));
                action_func();
                return false;
            default:
                normal_mode();
                return false;
        }
    }
    return false;
}
#endif

#ifdef _VIM_TEXT_OBJECTS
bool process_text_objects(uint16_t keycode, const keyrecord_t *record) {
    if (record->event.pressed) {
#ifdef VIM_I_TEXT_OBJECTS
        if (keycode == KC_I) {
            process_func = process_in_object;
            return false;
        }
#endif
#ifdef VIM_A_TEXT_OBJECTS
        if (keycode == KC_A) {
            process_func = process_around_object;
            return false;
        }
#endif
    }
    return true;
}
#endif // end ifdef VIM_TEXT_OBJECTS

// The actual change action
void change_action(void) {
    tap_code16(VIM_CHANGE);
    insert_mode();
}
// The actual delete action
void delete_action(void) {
    yanked_line = false;
    tap_code16(VIM_DELETE);
    normal_mode();
}
// The delete action for a line
void delete_line_action(void) {
    yanked_line = true;
    tap_code16(VIM_DELETE);
    normal_mode();
}
// The actual yank action
void yank_action(void) {
    yanked_line = false;
    tap_code16(VIM_YANK);
    tap_code(KC_LEFT);
    normal_mode();
}
// The yank action for a line
void yank_line_action(void) {
    yanked_line = true;
    tap_code16(VIM_YANK);
    tap_code(KC_LEFT);
    normal_mode();
}
// The paste action
void paste_action(void) {
    if (yanked_line) {
        VIM_END();
        tap_code(KC_RIGHT);
    }
    tap_code16(VIM_PASTE);
    normal_mode();
}
#ifdef VIM_PASTE_BEFORE
// Paste before, ie P
void paste_before_action(void) {
    if (yanked_line) {
        VIM_END();
        tap_code(KC_RIGHT);
        tap_code(KC_UP);
    }
    else {
        tap_code(KC_LEFT);
    }
    tap_code16(VIM_PASTE);
    normal_mode();
}
#endif

// Function to start a change action
void start_change_action(void) {
    action_key = KC_C;
    action_func = change_action;
    process_func = process_vim_action;
}
// Function to start a delete action
void start_delete_action(void) {
    action_key = KC_D;
    action_func = delete_action;
    process_func = process_vim_action;
}
// Function to start a yank action
void start_yank_action(void) {
    action_key = KC_Y;
    action_func = yank_action;
    process_func = process_vim_action;
}

// a little nasty but okay...
extern bool process_visual_mode(uint16_t keycode, const keyrecord_t *record);
static void visual_no_action(void) {
    process_func = process_visual_mode;
}
// Function to start a visual action
void start_visual_action(void) {
    action_key = KC_NO;
    action_func = visual_no_action;
    // Don't update the process_func, that should already be set to the
    // visual mode process func
}

#ifdef VIM_REPLACE
static bool process_replace_action(uint16_t keycode, const keyrecord_t *record) {
    if (record->event.pressed) {
        if (keycode != KC_ESC) {
            tap_code(KC_DELETE);
            tap_code16(keycode);
            tap_code(KC_LEFT);
        }
        normal_mode();
    }
    return false;
}

void replace_action(void) {
    process_func = process_replace_action;
}
#endif

#ifdef VIM_DOT_REPEAT

#ifndef VIM_REPEAT_BUF_SIZE
#define VIM_REPEAT_BUF_SIZE 64
#endif

typedef enum {
    INVALID_REPEAT = 0,
    VALID_REPEAT,
    RECORDING_REPEAT,
    EXECUTING_REPEAT,
} repeat_state_t;

static repeat_state_t repeat_state = INVALID_REPEAT;
static uint8_t repeat_buf_idx = 0;
// if this gets much bigger you would probably want it in progmem
static uint16_t repeat_buf[VIM_REPEAT_BUF_SIZE];

void start_recording_repeat(void) {
    // if the state isn't recording or executing
    if (repeat_state <= VALID_REPEAT) {
        repeat_state = RECORDING_REPEAT;
        repeat_buf_idx = 0;
    }
}

void add_repeat_keycode(uint16_t keycode) {
    if (repeat_state == RECORDING_REPEAT) {
        if (repeat_buf_idx < VIM_REPEAT_BUF_SIZE) {
            repeat_buf[repeat_buf_idx] = keycode;
            ++repeat_buf_idx;
            if (process_func == process_normal_mode) {
                repeat_state = VALID_REPEAT;
            }
        }
        else {
            repeat_state = INVALID_REPEAT;
        }
    }
}

void repeat_action(const keyrecord_t *record) {
    if (repeat_state == VALID_REPEAT) {
        repeat_state = EXECUTING_REPEAT;
        for (uint8_t i = 0; i < repeat_buf_idx; ++i) {
            if (process_func(repeat_buf[i], record)) {
                tap_code16(repeat_buf[i]);
            }
        }
        repeat_state = VALID_REPEAT;
    }
}

#endif
