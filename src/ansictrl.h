#pragma once

#include <stdlib.h> // for getenv
#include <stdbool.h>

#if (defined _WIN32 || defined _WIN64 || defined __WINDOWS__)
    #define PLATFORM_WIN 1
#else
    #define PLATFORM_WIN 0
#endif


#if PLATFORM_WIN == 1

#define ENTER 13
#define ESCAPE 27
#define SPACE_BAR 32
#define BACK_SPACE 8

// specials
#define LEFT 75
#define RIGHT 77
#define UP 72
#define DOWN 80

#define HOME 71
#define END 79
#define DELETE 83

#else

#define ENTER 10
#define ESCAPE 27
#define SPACE_BAR 32
#define BACK_SPACE 127

// specials
#define LEFT 68
#define RIGHT 67
#define UP 65
#define DOWN 66

#define HOME 72
#define END 70
#define DELETE 126


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
#define COLOR_GRAY "\033[90m"
#define COLOR_CYAN "\033[96m"

#define COLOR_BOLD "\033[1m"
#define COLOR_NORMAL "\033[0m"
#define COLOR_INVERSE "\033[7m"

#define COLOR_RESET "\033[m"


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

char readkey(bool* special);
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
