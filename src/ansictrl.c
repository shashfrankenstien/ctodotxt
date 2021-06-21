

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "ansictrl.h"



#if PLATFORM_WIN == 1

#define __ENTER 13
#define __ESCAPE 27
#define __BACK_SPACE 8

// multibyte
#define __LEFT 75
#define __RIGHT 77
#define __UP 72
#define __DOWN 80

#define __HOME 71
#define __END 79
#define __DELETE 83

#else

#define __ENTER 10
#define __ESCAPE 27
#define __BACK_SPACE 127

// multibyte
#define __LEFT 68
#define __RIGHT 67
#define __UP 65
#define __DOWN 66

#define __HOME 72
#define __END 70
#define __DELETE 126

#endif



// platform specific 'readkey' and 'get_console_size' definitions

#if PLATFORM_WIN == 1

#include <conio.h>
#include <windows.h>

static char readkey(bool* multibyte)
{
	bool _multibyte = false;
	char c = _getch();
	if (c==0 || c == -32) {
		int nbbytes = _kbhit(); // check if there are any pending bits
		for (int i=0; i<nbbytes; i++) {
			c = _getch(); // multibyte chars
			_multibyte = true;
		}
	}
	*multibyte = _multibyte;
	return c;
}

int get_console_size(int* lines, int* cols)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    *cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    *lines = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    return 0;
}

#else

#include <termios.h>
#include <sys/ioctl.h> // For FIONREAD, TIOCGSIZE / TIOCGWINSZ


static struct termios old, new;

static void init_termios()
{
	tcgetattr(0, &old);
	new = old;
	new.c_lflag &= ~ICANON;
	new.c_lflag &= ~ECHO;    /* set no echo mode */
	tcsetattr(0, TCSANOW, &new);    /* use these new terminal i/o settings now */
	setbuf(stdin, NULL);
}

static void reset_termios(void) /* Restore old terminal i/o settings */
{
	tcsetattr(0, TCSANOW, &old);
}

static char readkey(bool* multibyte) /* Read 1 character */
{
	init_termios();
	char c = getchar();
	bool _multibyte = false;

	if (c==27) {
		int nbbytes;
		ioctl(0, FIONREAD, &nbbytes); // check if there are any pending bits
		for (int i=0; i<nbbytes; i++) {
			c = getchar(); // multibyte chars
			_multibyte = true;
		}
	}
	reset_termios();
	*multibyte = _multibyte;
	return c;
}


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

#endif


// common getkey
int read_keypress()
{
    int ch;
    bool multibyte;
    ch = readkey(&multibyte);
    if (multibyte) {
        switch(ch) {
            case __LEFT:
                return LEFT;
                break;
            case __RIGHT:
                return RIGHT;
                break;
            case __UP:
                return UP;
                break;
            case __DOWN:
                return DOWN;
                break;
            case __HOME:
                return HOME;
                break;
            case __END:
                return END;
                break;
            case __DELETE:
                return DELETE;
                break;
            default:
                return UNKNOWN;
                break;
        }
    }
    else {
        switch(ch) {
            case __ENTER:
                return ENTER;
                break;
            case __ESCAPE:
                return ESCAPE;
                break;
            case __BACK_SPACE:
                return BACK_SPACE;
                break;
        }
    }
    return ch; // return ch if all else fails
}


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
void console_clear_eol() // Erase from cursor to end of line
{
    printf(CLEAR_EOL_CODE);
}

void console_clear_sol() // Erase from start of line to cursor
{
    printf(CLEAR_SOL_CODE);
}

void console_clear_line() // Erase whole line
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
