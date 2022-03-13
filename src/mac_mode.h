#pragma once

#include QMK_KEYBOARD_H

#ifdef VIM_FOR_ALL

extern bool vim_for_mac;

#define VIM_MAC_NOMAC(a, b) (vim_for_mac ? (a) : (b))
// The results of this implimentation of VIM_MAC_NOMAC are not constant
// expressions, which means they cannot be used in a case statement
// In order to get around this restriction we can duplicate the switch statement
// and force our keycode to evaluate as a constant expression in each
#define VIM_REDUCE_MAC_NOMAC_TO_CONST(body) do { \
    if(vim_for_mac) { \
        const bool vim_for_mac = true; \
        body \
    } \
    else { \
        const bool vim_for_mac = false; \
        body \
    } \
} while (0)

#else

#ifdef VIM_FOR_MAC
#define VIM_MAC_NOMAC(a, b) (a)

#else
#define VIM_MAC_NOMAC(a, b) (b)
#endif

#define VIM_REDUCE_MAC_NOMAC_TO_CONST(body) do { \
    body \
} while(0)

#endif

