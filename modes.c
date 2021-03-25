#include "modes.h"
#include "motions.h"
#include "process_func.h"

extern process_func_t process_func;

#ifdef BETTER_VISUAL_MODE
extern visual_direction_t visual_direction;
#endif

// The function that handles normal mode keycode inputs
static bool process_normal_mode(uint16_t keycode, const keyrecord_t *record) {
    // handle motions on their own so they can be pressed and held
    if (!process_motions(keycode, record, 0)) {
        return false;
    }
    if (record->event.pressed) {
        switch (keycode) {
            // insert keys
            case LSFT(KC_I):
                tap_code16(KC_HOME);
                // fallthrough to lowercase i
            case KC_I:
                insert_mode();
                return false;
            case LSFT(KC_A):
                tap_code16(KC_END);
                insert_mode();
                return false;
            case KC_A:
                tap_code16(KC_RIGHT);
                insert_mode();
                return false;
            case LSFT(KC_O):
                tap_code16(KC_UP);
                // fallthrough to lowercase o
            case KC_O:
                tap_code16(KC_END);
                tap_code16(KC_ENTER);
                insert_mode();
                return false;
            // g motions
            case LSFT(KC_G):
                // this doesn't quite work for all programs
                tap_code16(VCMD(KC_A));
                wait_ms(200);
                tap_code16(KC_DOWN);
                return false;
            // visual modes
            case LSFT(KC_V):
                visual_line_mode();
                return false;
            case KC_V:
                visual_mode();
                return false;
            // undo redo
            case KC_U:
                tap_code16(VCMD(KC_Z));
                return false;
            case LCTL(KC_R):
                tap_code16(VCMD(KC_Y));
                return false;
            case KC_P:
                tap_code16(KC_END);
                tap_code16(KC_RIGHT);
                tap_code16(LCTL(KC_V));
                return false;
            default:
                break;
        }
    }
    return false;
}

// The function that handles visual mode keycode inputs
static bool process_visual_mode(uint16_t keycode, const keyrecord_t *record) {
    // handle motions on their own so they can be pressed and held
    if (!process_motions(keycode, record, QK_LSFT)) {
        return false;
    }
    if (record->event.pressed) {
        switch (keycode) {
            case KC_ESC:
#ifdef BETTER_VISUAL_MODE
                if (visual_direction == V_FORWARD)
                    tap_code16(KC_RIGHT);
                else
                    tap_code16(KC_LEFT);
#else
                tap_code16(KC_RIGHT);
#endif
                normal_mode();
                return false;
            default:
                break;
        }
    }
    return false;
}

// The function that handles visual line mode keycode inputs
static bool process_visual_line_mode(uint16_t keycode, const keyrecord_t *record) {
    // handle motions on their own so they can be pressed and held
    switch (keycode) {
        case KC_J:
        case VIM_J:
#ifdef BETTER_VISUAL_MODE
            if (visual_direction == V_NONE) {
                tap_code16(KC_LEFT);
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
            case KC_Y:
                tap_code16(LCTL(KC_C));
                tap_code16(KC_LEFT);
                normal_mode();
                return false;
            case KC_ESC:
#ifdef BETTER_VISUAL_MODE
                if (visual_direction == V_FORWARD)
                    tap_code16(KC_RIGHT);
                else
                    tap_code16(KC_LEFT);
#else
                tap_code16(KC_RIGHT);
#endif
                normal_mode();
                return false;
            default:
                break;
        }
    }
    return false;
}

// The function that handles insert mode keycode inputs
static bool process_insert_mode(uint16_t keycode, const keyrecord_t *record) {
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


// Function to enter into normal mode
void normal_mode(void) {
    process_func = process_normal_mode;
}

// Function to enter into insert mode
void insert_mode(void) {
    // need to clear motion keys if they are currently pressed
    /* clear_keyboard(); */
    process_func = process_insert_mode;
}

// Function to enter into visual mode
void visual_mode(void) {
#ifdef BETTER_VISUAL_MODE
    visual_direction = V_NONE;
#endif
    process_func = process_visual_mode;
}


// Function to enter into visual line mode
void visual_line_mode(void) {
#ifdef BETTER_VISUAL_MODE
    visual_direction = V_NONE;
#endif
    tap_code16(KC_END);
    tap_code16(KC_RIGHT);
    tap_code16(LSFT(KC_UP));

    process_func = process_visual_line_mode;
}
