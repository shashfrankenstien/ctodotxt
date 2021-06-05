

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "ansictrl.h"



int colorize(char* dest, char* src, ColorCodes c, bool bold)
{
    int _bold = (bold==true) ? 1 : 0;
    return snprintf(dest, strlen(src)+strlen(COLORIZE_TMPL), COLORIZE_TMPL, c, _bold, src);
}



#if PLATFORM_WIN == 1

#include <windows.h>

int get_console_size(int* lines, int* cols)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    *cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    *lines = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    return 0;
}

#else

#include <sys/ioctl.h> // For TIOCGSIZE / TIOCGWINSZ

int get_console_size(int* lines, int* cols)
{
    #ifdef TIOCGSIZE
    struct ttysize ts;
    ioctl(0, TIOCGSIZE, &ts);
    *cols = ts.ts_cols;
    *lines = ts.ts_lines;
    #elif defined(TIOCGWINSZ)
    struct winsize ts;
    ioctl(0, TIOCGWINSZ, &ts);
    *lines = ts.ws_row;
    *cols = ts.ws_col;
    #endif /* TIOCGSIZE */
    return 0;
}

#endif // PLATFORM_WIN == 1


// navigation

void console_clear_screen()
{
    if (supports_ansii()) {
        printf(CLEAR_SCREEN_CODE CURSOR_POSITION_TMPL, 0, 0);
    }
    else {
        #if PLATFORM_WIN == 1
            system("cls");
        #else
            system("clear");
        #endif
    }
}

void console_clear_line()
{
    printf(CLEAR_LINE_CODE);
}

void cursor_position(int l, int c)
{
    printf(CURSOR_POSITION_TMPL, l, c);
}

void cursor_mv_up(int n)
{
    printf(CURSOR_MOVE_UP_TMPL, n);
}

void cursor_mv_down(int n)
{
    printf(CURSOR_MOVE_DOWN_TMPL, n);
}

void cursor_mv_right(int n)
{
    printf(CURSOR_MOVE_RIGHT_TMPL, n);
}

void cursor_mv_left(int n)
{
    printf(CURSOR_MOVE_LEFT_TMPL, n);
}
