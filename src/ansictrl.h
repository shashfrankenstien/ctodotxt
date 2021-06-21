#pragma once

#include <stdlib.h> // for getenv
#include <stdbool.h>

#if (defined _WIN32 || defined _WIN64 || defined __WINDOWS__)
    #define PLATFORM_WIN 1
#else
    #define PLATFORM_WIN 0
#endif


#define supports_ansii() ((getenv("ANSICON") || PLATFORM_WIN==0) ? true : false)

// colors

#define COLOR_RED "\033[31m"
#define COLOR_GREEN "\033[32m"
#define COLOR_YELLOW "\033[93m" //#33
#define COLOR_BLUE "\033[34m"
#define COLOR_PINK "\033[35m"
#define COLOR_LIGHT_BLUE "\033[36m"
#define COLOR_WHITE "\033[37m"
#define COLOR_CYAN "\033[96m"
#define COLOR_GRAY "\033[90m"
#define COLOR_LIGHT_GRAY "\033[38;2;220;220;220m" // RGB color

#define COLOR_NORMAL "\033[22m"
#define COLOR_BOLD "\033[1m"
#define COLOR_UNDERLINED "\033[4m"
#define COLOR_STRIKE "\033[9m"
#define COLOR_INVERSE "\033[7m"

#define COLOR_RESET "\033[0m"


// navigation

#define CLEAR_EOL_CODE "\033[K" // Erase from cursor to end of line
#define CLEAR_SOL_CODE "\033[1K" // Erase from start of line to cursor
#define CLEAR_LINE_CODE "\033[2K" // Erase whole line
#define CLEAR_SCREEN_CODE "\033[2J"

#define CURSOR_POSITION_TMPL "\033[%d;%dH"
#define CURSOR_MOVE_UP_TMPL "\033[%dA"
#define CURSOR_MOVE_DOWN_TMPL "\033[%dB"
#define CURSOR_MOVE_RIGHT_TMPL "\033[%dC"
#define CURSOR_MOVE_LEFT_TMPL "\033[%dD"


enum SPECIAL_KEYS {
    ENTER = 1000,
    ESCAPE,
    BACK_SPACE,
    // multibyte
    LEFT,
    RIGHT,
    UP,
    DOWN,
    HOME,
    END,
    DELETE,
    UNKNOWN
};


int read_keypress();
int get_console_size(int* lines, int* cols);

void console_clear_screen();
void console_clear_line();
void console_clear_eol();
void console_clear_sol();

void cursor_position(int l, int c);
void cursor_mv_up(int n);
void cursor_mv_down(int n);
void cursor_mv_right(int n);
void cursor_mv_left(int n);
