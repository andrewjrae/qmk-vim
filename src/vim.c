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
#include "modes.h"
#include "actions.h"
#include "process_func.h"

// the current process func, from in process_func.c
extern process_func_t process_func;

static bool vim_enabled = false;

#ifdef VIM_FOR_ALL
// Check to see if mac mode is enabled
bool vim_for_mac_enabled(void) {
    return vim_for_mac;
}
// Enable mac mode
void enable_vim_for_mac(void) {
    vim_for_mac = true;
}
// Disable mac mode
void disable_vim_for_mac(void) {
    vim_for_mac = false;
}
// Toggle mac mode
void toggle_vim_for_mac(void) {
    vim_for_mac = !vim_for_mac;
}
#endif

// Check to see if vim mode is enabled
bool vim_mode_enabled(void) {
    return vim_enabled;
}
// Enable vim mode
void enable_vim_mode(void) {
    vim_enabled = true;
    normal_mode();
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

#ifdef ONESHOT_VIM
extern bool process_normal_mode(uint16_t keycode, const keyrecord_t *record);
extern bool process_insert_mode(uint16_t keycode, const keyrecord_t *record);

static bool oneshot_vim_enabled = false;
static process_func_t last_process_func;
// Start vim mode
void start_oneshot_vim(void) {
    oneshot_vim_enabled = true;
    last_process_func = process_normal_mode;
    enable_vim_mode();
}
// Stop vim mode
void stop_oneshot_vim(void) {
    oneshot_vim_enabled = false;
    disable_vim_mode();
}
static inline void vim_oneshot_termination(uint16_t keycode) {
    if (oneshot_vim_enabled &&
        (keycode == KC_ESC ||
            process_func == process_insert_mode ||
            (process_func == process_normal_mode && last_process_func != process_func))) {
        stop_oneshot_vim();
    }
    last_process_func = process_func;
}
#endif

// Process keycode for leader sequences
bool process_vim_mode(uint16_t keycode, const keyrecord_t *record) {
    if (vim_enabled) {
        // Get the base keycode of a mod or layer tap key
        if ((QK_MOD_TAP <= keycode && keycode <= QK_MOD_TAP_MAX)
            || (QK_LAYER_TAP <= keycode && keycode <= QK_LAYER_TAP_MAX)) {
            // Earlier return if this has not been considered tapped yet
            if (record->tap.count == 0)
                return true;
            keycode = keycode & 0xFF;
        }

        // let through anything above normal keyboard keycode or a mod
        if ((keycode < KC_A || keycode > KC_CAPS_LOCK) && (keycode < QK_MODS || keycode > QK_MODS_MAX)) {
            return true;
        }

        const uint8_t mods = get_mods();
        const uint8_t oneshot_mods = get_oneshot_mods();
        // hoping this gets optimized away by the compiler
        const uint8_t all_mods = mods | oneshot_mods;
        // this takes mod bits adds them to to the keycode, but always as the left mod (lower 4 mod bits)
        // for some reason with AVR gcc 8.3.0, the compile size is larger if you use a |= ???
        keycode = all_mods & 0xF0 ? keycode | (all_mods << 4) : keycode | (all_mods << 8);

        // clear the mods
        clear_mods();
        clear_oneshot_mods();

        // process the current keycode
        bool do_process_key = process_func(keycode, record);

#ifdef VIM_DOT_REPEAT
        if (record->event.pressed) {
            add_repeat_keycode(keycode);
        }
#endif

        set_mods(mods);
        // only restore one shot mods if the key was passed through
        if (do_process_key) {
            set_oneshot_mods(oneshot_mods);
        }

#ifdef ONESHOT_VIM
        // this function checks for oneshot termination conditions and stops if applicable
        vim_oneshot_termination(keycode);
#endif

        return do_process_key;
    }
    return true;
}
