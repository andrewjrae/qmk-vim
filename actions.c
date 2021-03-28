#include "actions.h"
#include "motions.h"
#include "process_func.h"

// Function pointer type for executing actions
typedef void (*action_func_t)(void);

// the current process func, from in process_func.c
extern process_func_t process_func;

// forward declare insert_mode
void normal_mode(void);
void insert_mode(void);

// which key is the current action key
static uint16_t action_key;
// the current action_function
static action_func_t action_func;

// Function to process key codes when in an action sequence
static bool process_vim_action(uint16_t keycode, const keyrecord_t *record) {
    // handle double taps, ie cc, yy, dd
    if (record->event.pressed) {
        if (keycode == action_key) {
            // for the change action
            if (action_func == change_action) {
                tap_code16(KC_HOME);
                tap_code16(LSFT(KC_END));
            }
            else {
                tap_code16(KC_END);
                tap_code16(KC_RIGHT);
                tap_code16(LSFT(KC_UP));
            }
            action_func();
            return false;
        }
    }

    if (!process_motions(keycode, record, QK_LSFT)) {
        clear_keyboard();
        action_func();
        return false;
    }

    if (!process_text_objects(keycode, record)) {
        return false;
    }

    return false;
}

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
                return true;
        }
    }
    return false;
}

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
                return true;
        }
    }
    return false;
}

bool process_text_objects(uint16_t keycode, const keyrecord_t *record) {
    if (record->event.pressed) {
        switch (keycode) {
            case KC_I:
                process_func = process_in_object;
                return false;
            case KC_A:
                process_func = process_around_object;
                return false;
            default:
                return true;
        }
    }
    return true;
}

// The actual change action
void change_action(void) {
    tap_code16(VIM_CHANGE);
    insert_mode();
}
// The actual delete action
void delete_action(void) {
    tap_code16(VIM_DELETE);
    normal_mode();
}
// The actual yank action
void yank_action(void) {
    tap_code16(VIM_YANK);
    tap_code16(KC_LEFT);
    normal_mode();
}


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
