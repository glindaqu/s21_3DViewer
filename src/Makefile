.PHONY: all build clean clang-format clang-format-check clear

CC = gcc
GCC_FLAGS = -Wall -Werror -Wextra -g
CHECK_FLAGS = $(shell pkg-config --cflags check) $(shell pkg-config --libs check)
GTK_FLAGS = $(shell pkg-config --cflags gtk4) $(shell pkg-config --libs gtk4)
UI_MODULES = modules/ui/resources.o modules/ui/viewer-app.o modules/ui/viewer-app-window.o modules/ui/viewer-error.o modules/ui/viewer-glfuncs.o

all: build clang-format

build:
	@$(MAKE) -C modules/parser
	@$(MAKE) -C modules/ui
	@$(MAKE) -C modules/matrix_calc
	$(CC) $(GTK_FLAGS) -I./include/ -L./include/ 3DViewer/main.c $(UI_MODULES) -o 3DViewer.out -lparser -lepoxy -lm -lmatrix_calc

test-parser:
	@$(MAKE) -C modules/parser
	@checkmk tests/parser/parser.check > tests/parser/parser.c
	@$(CC) $(GCC_FLAGS) $(CHECK_FLAGS) -I./include/ -L./include/ tests/parser/parser.c -o tests/parser/test-parser.out -lparser
	@./tests/parser/test-parser.out

clang-format-check:
	@find . -name "*.c" -o -name "*.h" -o -name "*.cpp" | xargs clang-format -n --style=Google 

clang-format:
	@find . -name "*.c" -o -name "*.h" -o -name "*.cpp" | xargs clang-format -i --style=Google

clean: clear

clear:
	@echo "Cleaning..."
	@$(MAKE) -C modules/parser clean
	@$(MAKE) -C modules/ui clean
	@$(MAKE) -C modules/matrix_calc clean
	@find . -name "*.o" -o -name "*.a" -o -name "*.out" | xargs rm -rf
	@rm -rf *.out
