#include "modes.h"
#include "motions.h"

process_func_t process_func = NULL;

#ifdef BETTER_VISUAL_MODE
extern visual_direction_t visual_direction;
#endif

static bool process_normal_mode(uint16_t keycode, const keyrecord_t *record) {
    // handle motions on their own so they can be pressed and held
    if (!process_motions(keycode, record, 0)) {
        return false;
    }
    if (record->event.pressed) {
        switch (keycode) {
            case LSFT(KC_G):
                // this doesn't quite work for all programs
                tap_code16(VCMD(KC_A));
                wait_ms(200);
                tap_code16(KC_DOWN);
                return false;
            case KC_U:
                tap_code16(VCMD(KC_Z));
                return false;
            case KC_V:
                visual_mode();
                return false;
            case LSFT(KC_V):
                visual_line_mode();
                return false;
            case LCTL(KC_R):
                tap_code16(VCMD(KC_Y));
                return false;
            default:
                break;
        }
    }
    return false;
}

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

static bool process_visual_line_mode(uint16_t keycode, const keyrecord_t *record) {
    // handle motions on their own so they can be pressed and held
    switch (keycode) {
        case KC_J:
        case VIM_J:
#ifdef BETTER_VISUAL_MODE
            if (visual_direction == V_NONE) {
                tap_code16(KC_HOME);
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

void normal_mode(void) {
    process_func = process_normal_mode;
}

void insert_mode(void) {
    process_func = process_normal_mode;
}

void visual_mode(void) {
#ifdef BETTER_VISUAL_MODE
    visual_direction = V_NONE;
#endif
    process_func = process_visual_mode;
}


void visual_line_mode(void) {
#ifdef BETTER_VISUAL_MODE
    visual_direction = V_NONE;
    tap_code16(KC_END);
    tap_code16(LSFT(KC_HOME));
#else
    tap_code16(KC_HOME);
#endif

    process_func = process_visual_line_mode;
}
