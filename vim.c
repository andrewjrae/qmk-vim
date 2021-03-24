/* Copyright 2021 Andrew Rae ajrae.nv@gmail.com @andrewjrae
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "vim.h"
#include "motions.h"

typedef bool (*process_func_t)(uint16_t, const keyrecord_t*);

static bool vim_enabled = false;
static process_func_t process_func = NULL;

static bool process_normal_mode(uint16_t keycode, const keyrecord_t *record) {
    // handle these on their own so they can be pressed and held
    switch (keycode) {
        case KC_H:
            register_motion(VIM_H, record);
            return false;
        case KC_J:
            register_motion(VIM_J, record);
            return false;
        case KC_K:
            register_motion(VIM_K, record);
            return false;
        case KC_L:
            register_motion(VIM_L, record);
            return false;
        case KC_B:
            register_motion(VIM_B, record);
            return false;
        case KC_W:
            register_motion(VIM_W, record);
            return false;
        default:
            break;
    }
    if (record->event.pressed) {
        switch (keycode) {
            case KC_0:
                tap_code16(VIM_0);
                return false;
            case KC_DLR:
                tap_code16(VIM_DLR);
                return false;
            case LSFT(KC_G):
                tap_code16(VCMD(KC_A));
                tap_code16(KC_DOWN);
                return false;
            case KC_U:
                tap_code16(VCMD(KC_Z));
                return false;
            case LCTL(KC_R):
                // redo is tricky, as it's really not standardized...
                tap_code16(VCMD(KC_Y));
                return false;
            default:
                break;
        }
    }
    return false;
}

// Check to see if vim mode is enabled
bool vim_mode_enabled(void) {
    return vim_enabled;
}
// Enable vim mode
void enable_vim_mode(void) {
    vim_enabled = true;
    process_func = process_normal_mode;
}
// Disable vim mode
void disable_vim_mode(void) {
    vim_enabled = false;
}
// Toggle vim mode
void toggle_vim_mode(void) {
    if (vim_enabled) {
        disable_vim_mode();
    }
    else {
        enable_vim_mode();
    }
}

// Process keycode for leader sequences
bool process_vim_mode(uint16_t keycode, const keyrecord_t *record) {
    if (vim_enabled) {
        // Get the base keycode of a mod or layer tap key
        switch (keycode) {
            case QK_MOD_TAP ... QK_MOD_TAP_MAX:
            case QK_LAYER_TAP ... QK_LAYER_TAP_MAX:
            case QK_TAP_DANCE ... QK_TAP_DANCE_MAX:
                // Earlier return if this has not been considered tapped yet
                if (record->tap.count == 0)
                    return true;
                keycode = keycode & 0xFF;
                break;
            default:
                break;
        }

        // let through anything above normal keyboard keycode or a mod
        if (keycode > QK_MODS_MAX || IS_MOD(keycode)
            || (keycode >= KC_SYSTEM_POWER && keycode <= KC_MS_ACCEL2)) {
            return true;
        }

        // deal with mods
        static uint8_t mods = 0;
        static uint8_t all_mods = 0;
        mods = get_mods();
        all_mods = mods | get_oneshot_mods();
        if (all_mods & MOD_MASK_SHIFT) {
            keycode = LSFT(keycode);
        }
        else if (all_mods & MOD_MASK_CTRL) {
            keycode = LCTL(keycode);
        }
        // TODO: allow for configuration here?
        // let through alt and gui chords
        else if (all_mods) {
            return true;
        }

        // clear the mods
        clear_mods();
        clear_oneshot_mods();

        // process the current keycode
        bool do_process_key = process_func(keycode, record);

        // don't restore one shot mods as they have run their course
        set_mods(mods);

        return do_process_key;
    }
    return true;
}
