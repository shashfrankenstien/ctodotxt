#pragma once

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

typedef enum {
	RED = 31,
	GREEN = 32,
	YELLOW = 93, //#33
	BLUE = 34,
	PINK = 35,
	LIGHT_BLUE = 36,
	WHITE = 37,
	GRAY = 90,
	CYAN = 96,
} ColorCodes;

#define COLORIZE_TMPL "\033[%d;%dm%s\033[0m"

int colorize(char* dest, char* src, ColorCodes c, bool bold);


// navigation

#define CLEAR_LINE_CODE "\033[K"
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

void cursor_position(int l, int c);
void cursor_mv_up(int n);
void cursor_mv_down(int n);
void cursor_mv_right(int n);
void cursor_mv_left(int n);
