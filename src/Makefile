#Main Makefile 
.PHONY: clean clang-format clang-format-check

CC = gcc
# GCC_FLAGS = -Wall -Werror -Wextra
GTK_FLAGS = $(shell pkg-config --cflags gtk4) $(shell pkg-config --libs gtk4)

all: build

build: 
	echo "Building... UI"
	@make -C modules/ui


clang-format-check:
	@find . -name "*.c" -o -name "*.h" -o -name "*.cpp" | xargs clang-format -n --style=Google 

clang-format:
	@find . -name "*.c" -o -name "*.h" -o -name "*.cpp" | xargs clang-format -i --style=Google

clean: clear

clear:
	@find . -name "*.o" -o -name "obj" -o -name "*.a" | xargs rm -rf
	@rm -rf *.out
