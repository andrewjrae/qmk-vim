#include "motions.h"

#ifdef BETTER_VISUAL_MODE
visual_direction_t visual_direction = V_NONE;
#endif

void set_visual_direction(visual_direction_t dir) {
#ifdef BETTER_VISUAL_MODE
    if (visual_direction == V_NONE)
        visual_direction = dir;
#endif
}

static uint16_t motion_counter = 0;
void register_motion(uint16_t keycode, const keyrecord_t *record) {
    if (record->event.pressed) {
#ifdef VIM_NUMBERED_JUMPS
        if (motion_counter > 1) {
            tap_code16(keycode);
            return;
        }
#endif
        register_code16(keycode);
    } else {
        unregister_code16(keycode);
    }
}

vim_motion_processed_t process_motions(uint16_t keycode, const keyrecord_t *record, uint16_t qk_mods) {
#ifdef VIM_NUMBERED_JUMPS
    if (keycode >= KC_1 && keycode <= KC_0) {
        if (record->event.pressed) {
            motion_counter *= 10;
            if (keycode != KC_0) {
                motion_counter += keycode - KC_1 + 1;
            }
        }
        if (motion_counter > 0) {
            return NUMBER_PROCESSED;
        }
    }
    // make motion counter 1 if it is 0
    motion_counter = motion_counter ? motion_counter : 1;
    // note that the directions don't do anything unless BETTER_VISUAL_MODE is defined
    for (int i = 0; i < motion_counter; i++) {
#endif
        switch (keycode) {
        case KC_H:
        case VIM_H:
            set_visual_direction(V_BACKWARD);
            register_motion(qk_mods | VIM_H, record);
            break;
        case KC_J:
        case VIM_J:
            set_visual_direction(V_FORWARD);
            register_motion(qk_mods | VIM_J, record);
            break;
        case KC_K:
        case VIM_K:
            set_visual_direction(V_BACKWARD);
            register_motion(qk_mods | VIM_K, record);
            break;
        case KC_L:
        case VIM_L:
            set_visual_direction(V_FORWARD);
            register_motion(qk_mods | VIM_L, record);
            break;
        case KC_B:
        case VIM_B:
        case LSFT(KC_B):
            set_visual_direction(V_BACKWARD);
            register_motion(qk_mods | VIM_B, record);
            break;
        case KC_W:
        case VIM_W:
        case LSFT(KC_W):
#ifdef VIM_W_BEGINNING_OF_WORD
            set_visual_direction(V_FORWARD);
            register_motion(qk_mods | VIM_W, record);
            if (!record->event.pressed) {
                /* unregister_code16(qk_mods | VIM_W); */
                tap_code16(qk_mods | VIM_W);
                tap_code16(qk_mods | VIM_B);
            }
            break;
#endif
        case KC_E:
        case LSFT(KC_E):
            set_visual_direction(V_FORWARD);
            register_motion(qk_mods | VIM_E, record);
            break;
        case KC_0:
        case VIM_0:
        case KC_CIRC: // ^
            set_visual_direction(V_BACKWARD);
            register_motion(qk_mods | VIM_0, record);
            break;
        case KC_DLR:
        case VIM_DLR: // $
            set_visual_direction(V_FORWARD);
            register_motion(qk_mods | VIM_DLR, record);
            break;
        default:
            motion_counter = 0;
            return NO_MOTION;
        }
#ifdef VIM_NUMBERED_JUMPS
    }
    motion_counter = 0;
#endif
    return MOTION_PROCESSED;
}
