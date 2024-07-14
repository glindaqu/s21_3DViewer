.PHONY: all build clear rebuild clean style test lib-gcov
LIB=../../include/libparser.a

all: build $(LIB)

build: makeObjDir
	@gcc -Wall -Wextra -Werror -std=c11 -g -c parser.c -o obj/parser.o

build-gcov: makeObjDir
	@gcc -Wall -Wextra -Werror -std=c11 -fprofile-arcs -ftest-coverage -c parser.c -o obj/parser.o

lib-gcov: build-gcov $(LIB)
	@ar rcs $(LIB) obj/parser.o

obj/parser.o: parser.c

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
