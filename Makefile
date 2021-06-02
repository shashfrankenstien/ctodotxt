APP := todo-txt

CC := gcc
INCLUDES := -I./src -I./vendor
CFLAGS := -Wall


all: deps
	$(CC) $(CFLAGS) $(INCLUDES) ./src/*.c ./vendor/*.o -o $(APP)


deps:
	@cd vendor && make


clean:
	-rm $(APP)
	-rm vendor/*.o
