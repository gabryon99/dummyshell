CC = gcc
TARGET = dummyshell
CFLAGS += -std=c11 -Wall -g -DDEBUG -D_DEFAULT_SOURCE

SRC = main.c src/dummyshell.c

$(TARGET): $(SRC) include/dummyshell.h
	$(CC) $(CFLAGS) -I./include -o $@ $(SRC)

.PHONY: clean

clean:
	rm -rf $(TARGET)