.PHONY: all build clear rebuild clean

all: build
	@gcc -Wall -Wextra -Werror parser.c -o parser_test

build: makeObjDir
	@gcc -Wall -Wextra -Werror -std=c11 -c parser.c -o obj/parser.o

rebuild: clear build

clear:
	@rm -rf obj

clean: clear

makeObjDir:
	@mkdir obj -p