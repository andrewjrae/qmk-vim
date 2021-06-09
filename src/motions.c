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

void register_motion(uint16_t keycode, const keyrecord_t *record) {
    if (record->event.pressed)
        register_code16(keycode);
    else
        unregister_code16(keycode);
}

bool process_motions(uint16_t keycode, const keyrecord_t *record, uint16_t qk_mods) {
    // note that the directions don't do anything unless BETTER_VISUAL_MODE is defined
    switch (keycode) {
        case KC_H:
        case VIM_H:
            set_visual_direction(V_BACKWARD);
            register_motion(qk_mods | VIM_H, record);
            return false;
        case KC_J:
        case VIM_J:
            set_visual_direction(V_FORWARD);
            register_motion(qk_mods | VIM_J, record);
            return false;
        case KC_K:
        case VIM_K:
            set_visual_direction(V_BACKWARD);
            register_motion(qk_mods | VIM_K, record);
            return false;
        case KC_L:
        case VIM_L:
            set_visual_direction(V_FORWARD);
            register_motion(qk_mods | VIM_L, record);
            return false;
        case KC_B:
        case VIM_B:
            set_visual_direction(V_BACKWARD);
            register_motion(qk_mods | VIM_B, record);
            return false;
        case KC_E:
            set_visual_direction(V_FORWARD);
            register_motion(qk_mods | VIM_E, record);
            return false;
        case KC_W:
        case VIM_W:
            set_visual_direction(V_FORWARD);
            register_motion(qk_mods | VIM_W, record);
            register_motion(qk_mods | VIM_W, record);
            register_motion(qk_mods | VIM_B, record);
            return false;
        case KC_0:
        case VIM_0:
            set_visual_direction(V_BACKWARD);
            register_motion(qk_mods | VIM_0, record);
            return false;
        case KC_DLR:
        case VIM_DLR:
            set_visual_direction(V_FORWARD);
            register_motion(qk_mods | VIM_DLR, record);
            return false;
        default:
            return true;
    }
}
