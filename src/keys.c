#include <stdio.h>

#include "keys.h"

#if PLATFORM_WIN == 1

#include <conio.h>

char readkey()
{
	char c = getch();
	if (c==0) {
		c = getch(); // special chars
	}
	return c;
}

#else

#include <termios.h>
#include <sys/ioctl.h> // For FIONREAD


static struct termios old, new;

static void init_termios(int echo)
{
	tcgetattr(0, &old);
	new = old;
	new.c_lflag &= ~ICANON;
	new.c_lflag &= echo ? ECHO : ~ECHO;    /* set echo mode */
	tcsetattr(0, TCSANOW, &new);    /* use these new terminal i/o settings now */
	setbuf(stdin, NULL);
}

/* Restore old terminal i/o settings */
static void reset_termios(void)
{
	tcsetattr(0, TCSANOW, &old);
}



/* Read 1 character - echo defines echo mode */
static char getch_(int echo) {
	char ch;
	init_termios(echo);
	ch = getchar();

	if (ch==27) {
		int nbbytes;
		ioctl(0, FIONREAD, &nbbytes); // check if there are any pending bits
		for (int i=0; i<nbbytes; i++)
			ch = getchar();
	}

	reset_termios();
	return ch;
}

/* Read 1 character without echo */
char readkey(void)
{
	return getch_(0);
}

/* Read 1 character with echo */
char e_readkey(void)
{
	return getch_(1);
}

#endif
