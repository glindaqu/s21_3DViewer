.PHONY: all build clear rebuild clean style test

LIB=../../include/libparser.a

all: build $(LIB)

build: makeObjDir
	@gcc -Wall -Wextra -Werror -std=c11 -c parser.c -o obj/parser.o

parser_test: obj/parser.o
	@gcc -Wall -Wextra -Werror obj/parser.o -o parser_test

$(LIB): obj/parser.o
	@ar rcs $(LIB) obj/parser.o

rebuild: clear build

clear:
	@rm -rf obj

clean: clear

makeObjDir:
	@mkdir obj -p

style:
	@clang-format --style=Google -i *.c *.h

test: parser_test
	@./parser_test