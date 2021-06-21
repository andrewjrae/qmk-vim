FROM qmkfm/qmk_firmware:latest

ENV MY_KEYMAP_PATH=/qmk_firmware/keyboards/uno/keymaps/qmk-vim-update-readme

WORKDIR /qmk_firmware

COPY keymap $MY_KEYMAP_PATH
COPY run.py /qmk_firmware/qmk_vim_update_readme.py

CMD python3 qmk_vim_update_readme.py
