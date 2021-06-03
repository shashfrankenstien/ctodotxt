APP := todo-txt

CC := gcc
INCLUDES := -I./src -I./vendor
CFLAGS := -Wall -g


all: deps $(APP)


deps:
	@cd vendor && make


$(APP): src/*
	$(CC) $(CFLAGS) $(INCLUDES) ./src/*.c ./vendor/*.o -o $(APP)


clean:
	-rm $(APP)
	cd vendor && make clean


purge: clean
	cd vendor && make purge