CC := gcc
CFLAGS := -O3 -Wall -Wextra -std=c99 -I.


all: re.o sort_r.h

re.o: re.c re.h
	$(CC) $(CFLAGS) -c re.c -o $@


re.c re.h &:
	-git clone https://github.com/kokke/tiny-regex-c re
	cp re/re.* .
	rm -rf re


sort_r.h:
# -git clone https://github.com/noporpoise/sort_r.git --branch 1.0.3 sort_r
	-git clone https://github.com/noporpoise/sort_r.git sort_r
	cp sort_r/sort_r.h .
	rm -rf sort_r


clean:
	-rm re.o


purge:
	-rm re.*
	-rm sort_r.h
