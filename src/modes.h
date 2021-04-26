#pragma once

typedef enum {
    NORMAL_MODE = 0,
    VISUAL_MODE,
    VISUAL_LINE_MODE,
    INSERT_MODE,
    UNKNOWN_MODE,
} vim_mode_t;

// Function to enter into normal mode
void normal_mode(void);
// Function to enter into insert mode
void insert_mode(void);
// Function to enter into visual mode
void visual_mode(void);
// Function to enter into visual line mode
void visual_line_mode(void);

// Get the mode that vim is currently in
vim_mode_t get_vim_mode(void);
