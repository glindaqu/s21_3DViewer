CC?=gcc
GCC_FLAGS = -Wall -Werror -Wextra -Wno-unused-function -g
CHECK_FLAGS = $(shell pkg-config --cflags check) $(shell pkg-config --libs check)
GTK_FLAGS = $(shell pkg-config --cflags gtk4) $(shell pkg-config --libs gtk4 cgif)
GCOV_FLAGS = -fprofile-arcs -ftest-coverage
UI_MODULES = modules/ui/resources.o modules/ui/viewer-app.o modules/file_processing/viewer-openFile.o \
modules/opengl/viewer-glarea.o modules/ui/viewer-setLabel.o modules/ui/viewer-app-window.o \
modules/ui/viewer-error.o modules/opengl/viewer-glfuncs.o modules/ui/viewer-app-settings.o \
modules/model_movement/viewer-modelMovement.o modules/settings/viewer-settings.o modules/file_processing/viewer-screenshots.o \
modules/file_processing/viewer-gif.o

INSTALL_DIR ?= ../../3DViewer
EXECUTABLE = 3DViewer.out
SCHEMA_PATH= modules/ui/

.PHONY: all build clean clang-format clang-format-check clear install uninstall

all: build clang-format

build: build-parser build-ui build-matrix_calc build-settings build-opengl build-file_processing build-modelMovement
	$(CC) $(GTK_FLAGS) -I./include/ -L./include/ 3DViewer/main.c $(UI_MODULES) -o $(EXECUTABLE) -lparser -lepoxy -lm -lmatrix_calc

run: all
	@echo "Running the application..."
	GSETTINGS_SCHEMA_DIR=modules/ui ./3DViewer.out


install: build $(EXECUTABLE)
	@echo "Installing the application..."
	@install -d $(INSTALL_DIR)
	@install $(EXECUTABLE) $(INSTALL_DIR)
	@bash ./gsettings.sh
	@echo "Application installed to $(INSTALL_DIR)"

uninstall:
	rm -rf $(INSTALL_DIR)

build-modelMovement:
	@$(MAKE) -C modules/model_movement

build-file_processing:
	@$(MAKE) -C modules/file_processing

build-opengl:
	@$(MAKE) -C modules/opengl

build-settings:
	@$(MAKE) -C modules/settings

build-parser:
	@$(MAKE) -C modules/parser

build-ui:
	@$(MAKE) -C modules/ui

build-matrix_calc:
	@$(MAKE) -C modules/matrix_calc

test-parser:
	@$(MAKE) -C modules/parser lib-gcov
	@checkmk tests/parser/parser.check > tests/parser/parser.c
	@$(CC) $(GCC_FLAGS) $(CHECK_FLAGS) -I./include/ -L./include/ tests/parser/parser.c -o tests/parser/test-parser.out -lparser -lgcov
	@./tests/parser/test-parser.out

gcov-parser: test-parser
	@lcov -t "test_coverage" -o test_coverage.info -c -d . --ignore-errors range
	@genhtml -o test/parser/report test_coverage.info --ignore-errors range

clang-format-check:
	@find . -name "*.c" -o -name "*.h" -o -name "*.cpp" | xargs clang-format -n --style=Google 

clang-format:
	@find . -name "*.c" -o -name "*.h" -o -name "*.cpp" | xargs clang-format -i --style=Google

clean: clear

clear:
	@echo "Cleaning..."
	@$(MAKE) -C modules/opengl clean
	@$(MAKE) -C modules/parser clean
	@$(MAKE) -C modules/file_processing clean
	@$(MAKE) -C modules/model_movement clean
	@$(MAKE) -C modules/settings clean
	@$(MAKE) -C modules/ui clean
	@$(MAKE) -C modules/matrix_calc clean
	@find . -name "*.o" -o -name "*.a" -o -name "*.out" -o -name "*.gcda" -o -name "*.gcno" | xargs rm -rf
	@rm -rf *.out
	@rm -rf tests/parser/parser.c
	@rm -rf test
	@rm -rf *.info

dvi: 
	@doxygen