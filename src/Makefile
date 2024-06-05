#Main Makefile 
.PHONY: clean clang-format clang-format-check

all:

git:
	@./tools/git.sh

clang-format-check:
	@find . -name "*.c" -o -name "*.h" -o -name "*.cpp" | xargs clang-format -n --style=Google 

clang-format:
	@find . -name "*.c" -o -name "*.h" -o -name "*.cpp" | xargs clang-format -i --style=Google

clean: clear

clear:
	@find . -name "*.o" | xargs rm -rf
	@rm -rf *.out
