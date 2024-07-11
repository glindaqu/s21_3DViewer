CC?=gcc
GCC_FLAGS = -Wall -Werror -Wextra -Wno-unused-function -g
CHECK_FLAGS = $(shell pkg-config --cflags check) $(shell pkg-config --libs check)
GTK_FLAGS = $(shell pkg-config --cflags gtk4) $(shell pkg-config --libs gtk4 cgif)
UI_MODULES = modules/ui/resources.o modules/ui/viewer-app.o modules/ui/viewer-openFile.o \
modules/ui/viewer-glarea.o modules/ui/viewer-setLabel.o modules/ui/viewer-app-window.o \
modules/ui/viewer-error.o modules/ui/viewer-glfuncs.o modules/ui/viewer-app-settings.o \
modules/ui/viewer-modelMovement.o modules/ui/viewer-settings.o modules/ui/viewer-screenshots.o \
modules/ui/viewer-gif.o
PREFIX ?= /usr/local
BINDIR = $(PREFIX)/bin
SHAREDIR = $(PREFIX)/share
ICONDIR = $(SHAREDIR)/icons/hicolor/scalable/apps
APPLICATIONSDIR = $(SHAREDIR)/applications
SCHEMADIR = $(SHAREDIR)/glib-2.0/schemas

EXECUTABLE = 3DViewer.out
ICON = school21.gdy._3dviewer.png
DESKTOP_FILE = your-app.desktop
SCHEMA_FILE = modules/ui/school21.gdy._3dviewer.gschema.xml

.PHONY: all build clean clang-format clang-format-check clear install uninstall

all: build clang-format

build: build-parser build-ui build-matrix_calc
	$(CC) $(GTK_FLAGS) -I./include/ -L./include/ 3DViewer/main.c $(UI_MODULES) -o $(EXECUTABLE) -lparser -lepoxy -lm -lmatrix_calc

run: build
	@echo "Running the application..."
	GSETTINGS_SCHEMA_DIR=modules/ui ./3DViewer.out

build-parser:
	@$(MAKE) -C modules/parser

build-ui:
	@$(MAKE) -C modules/ui

build-matrix_calc:
	@$(MAKE) -C modules/matrix_calc

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

install:
	@echo "Installing..."
	@install -Dm755 $(EXECUTABLE) $(DESTDIR)$(BINDIR)/main
	@install -Dm644 $(ICON) $(DESTDIR)$(ICONDIR)/$(ICON)
	@install -Dm644 $(DESKTOP_FILE) $(DESTDIR)$(APPLICATIONSDIR)/$(DESKTOP_FILE)
	@install -Dm644 $(SCHEMA_FILE) $(DESTDIR)$(SCHEMADIR)/$(SCHEMA_FILE)
	@glib-compile-schemas $(DESTDIR)$(SCHEMADIR)
	@update-icon-caches $(DESTDIR)$(ICONDIR)

uninstall:
	@echo "Uninstalling..."
	@rm -f $(DESTDIR)$(BINDIR)/main
	@rm -f $(DESTDIR)$(ICONDIR)/$(ICON)
	@rm -f $(DESTDIR)$(APPLICATIONSDIR)/$(DESKTOP_FILE)
	@rm -f $(DESTDIR)$(SCHEMADIR)/$(SCHEMA_FILE)
	@glib-compile-schemas $(DESTDIR)$(SCHEMADIR)
	@update-icon-caches $(DESTDIR)$(ICONDIR)