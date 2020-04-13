CC = gcc
TARGET = dummyshell
CFLAGS += -std=c11 -Wall -g -DDEBUG -D_DEFAULT_SOURCE

SRC = src/main.c src/dummyshell.c
INCLUDES = include/dummyshell.h

$(TARGET): $(SRC) $(INCLUDES)
	$(CC) $(CFLAGS) -I./include -o $@ $(SRC)

.PHONY: clean

clean:
	rm -rf $(TARGET)