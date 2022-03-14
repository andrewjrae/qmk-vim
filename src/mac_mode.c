#include "mac_mode.h"

#ifdef VIM_FOR_ALL

// When VIM_FOR_ALL is enabled, we allow the user to choose if mac support is enabled at startup or not
#ifdef VIM_FOR_MAC
bool vim_for_mac = true;
#else
bool vim_for_mac = false;
#endif

#endif
