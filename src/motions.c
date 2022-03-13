#include "mac_mode.h"
#include "motions.h"
#include "numbered_actions.h"

#ifdef BETTER_VISUAL_MODE
visual_direction_t visual_direction = V_NONE;
#endif

void set_visual_direction(visual_direction_t dir) {
#ifdef BETTER_VISUAL_MODE
    if (visual_direction == V_NONE)
        visual_direction = dir;
#endif
}

void register_motion(uint16_t keycode, const keyrecord_t *record) {
    if (record->event.pressed) {
#ifdef VIM_NUMBERED_JUMPS
        extern int16_t motion_counter;
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

bool process_motions(uint16_t keycode, const keyrecord_t *record, uint16_t qk_mods) {
    // This macro duplicates the code inside if VIM_FOR_ALL is enabled
    // Calling it on only a subset of the larger switch statement saves some space
    VIM_REDUCE_MAC_NOMAC_TO_CONST(
        switch (keycode) {
           case VIM_B:
               keycode = KC_B;
               break;
           case VIM_W:
               keycode = KC_W;
               break;
           case VIM_0:
               keycode = KC_0;
               break;
           case VIM_DLR:
               keycode = KC_DLR;
               break;
        }
    );
    DO_NUMBERED_ACTION(
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
        case LSFT(KC_B):
            set_visual_direction(V_BACKWARD);
            register_motion(qk_mods | VIM_B, record);
            break;
        case KC_W:
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
        case KC_CIRC: // ^
            set_visual_direction(V_BACKWARD);
            register_motion(qk_mods | VIM_0, record);
            break;
        case KC_DLR: // $
            set_visual_direction(V_FORWARD);
            register_motion(qk_mods | VIM_DLR, record);
            break;
        default:
            /* motion_counter = 0; */
            return true;
        }
    );
    return false;
}
