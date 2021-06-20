#include QMK_KEYBOARD_H
#include "qmk-vim/src/vim.h"

enum keycodes {
  QMK_VIM = SAFE_RANGE
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [0] = LAYOUT(QMK_VIM)
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  if (!process_vim_mode(keycode, record)) {
    return false;
  }
  if (keycode == QMK_VIM && record->event.pressed) {
    toggle_vim_mode();
    return false;
  }
  return true;
}
