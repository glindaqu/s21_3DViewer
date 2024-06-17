CC = gcc
GCC_FLAGS = -O3 -Wall -Werror -Wextra
GTK_FLAGS = $(shell pkg-config --cflags gtk4)
GTK_LIBS = $(shell pkg-config --libs gtk4)

all: build

build:
	$(CC) $(GCC_FLAGS) $(GTK_FLAGS) main.c -o main.out $(GTK_LIBS)

clean: clear

clear:
	rm -rf *.o
