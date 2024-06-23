.PHONY: all build clean clang-format clang-format-check clear

CC = gcc
GCC_FLAGS = -Wall -Werror -Wextra
GTK_FLAGS = $(shell pkg-config --cflags gtk4) $(shell pkg-config --libs gtk4)
UI_MODULES = modules/ui/resources.o modules/ui/viewer-app.o modules/ui/viewer-app-window.o modules/ui/viewer-error.o

all: build clang-format

build:
	@$(MAKE) -C modules/parser
	@$(MAKE) -C modules/ui
	$(CC) $(GTK_FLAGS) -I./include/ -L./include/ 3DViewer/main.c $(UI_MODULES) -o 3DViewer.out -lparser -lepoxy -lm

clang-format-check:
	@find . -name "*.c" -o -name "*.h" -o -name "*.cpp" | xargs clang-format -n --style=Google 

clang-format:
	@find . -name "*.c" -o -name "*.h" -o -name "*.cpp" | xargs clang-format -i --style=Google

clean: clear

clear:
	@echo "Cleaning..."
	$(MAKE) -C modules/parser clean
	$(MAKE) -C modules/ui clean
	@find . -name "*.o" -o -name "*.a" | xargs rm -rf
	@rm -rf *.out