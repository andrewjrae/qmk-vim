#pragma once

#include QMK_KEYBOARD_H

#ifdef VIM_FOR_ALL

extern bool vim_for_mac;

#define VIM_MAC_NOMAC(a, b) (vim_for_mac ? (a) : (b))
// The keycodes produced by this VIM_MAC_NOMAC are not constants, so they cannot be used in a case statement.
// In order to get around this, we can duplicate the switch statement
// and force our keycode to evaluate as a constant expression in each.
#define VIM_REDUCE_NOMAC_MAC_TO_CONST(body) { \
    if(vim_for_mac) { const bool vim_for_mac = true; body } \
    else { const bool vim_for_mac = false; body } \
}

#elif defined(VIM_FOR_MAC)
#define VIM_MAC_NOMAC(a, b) (a)

#else
#define VIM_MAC_NOMAC(a, b) (b)

#endif

