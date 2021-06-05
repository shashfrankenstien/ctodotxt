#include <stdio.h>

#include "keys.h"

#if PLATFORM_WIN == 1

#include <conio.h>

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

#else

#include <termios.h>
#include <sys/ioctl.h> // For FIONREAD


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

/* Restore old terminal i/o settings */
static void reset_termios(void)
{
	tcsetattr(0, TCSANOW, &old);
}



/* Read 1 character */
char readkey(bool* special)
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

#endif
