CC := gcc
CFLAGS := -O3 -Wall -Wextra -std=c99 -I.


all: re.o

re.o: re.c re.h
	$(CC) $(CFLAGS) -c re.c -o $@


re.c re.h &:
	-git clone https://github.com/kokke/tiny-regex-c re
	cp re/re.* .
	rm -rf re

clean:
	-rm re.*
