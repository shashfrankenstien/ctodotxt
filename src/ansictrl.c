

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "ansictrl.h"



// platform specific 'readkey' and 'get_console_size' definitions

#if PLATFORM_WIN == 1

#include <conio.h>
#include <windows.h>

char readkey(bool* special)
{
	bool _special = false;
	char c = _getch();
	if (c==0 || c == -32) {
		int nbbytes = _kbhit(); // check if there are any pending bits
		for (int i=0; i<nbbytes; i++) {
			c = _getch(); // special chars
			_special = true;
		}
	}
	*special = _special;
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

char readkey(bool* special) /* Read 1 character */
{
	init_termios();
	char c = getchar();
	bool _special = false;

	if (c==27) {
		int nbbytes;
		ioctl(0, FIONREAD, &nbbytes); // check if there are any pending bits
		for (int i=0; i<nbbytes; i++) {
			c = getchar(); // special chars
			_special = true;
		}
	}
	reset_termios();
	*special = _special;
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
