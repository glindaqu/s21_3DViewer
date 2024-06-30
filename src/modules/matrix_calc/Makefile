# Compiler and flags
GCC = gcc
GCC_FLAGS = -Wall -Werror -Wextra -O3
COMPILE_FLAGS = -c
LIB_FLAGS = -lm

OBJ_DIR = obj

LIB = ../../include/libmatrix_calc.a

SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c, $(OBJ_DIR)/%.o, $(SRCS))

all: build $(LIB)

build: makeObjDir $(OBJS)

$(OBJ_DIR)/%.o: %.c
	@$(GCC) $(GCC_FLAGS) $(COMPILE_FLAGS) $< -o $@ $(LIB_FLAGS)

$(LIB): $(OBJS)
	@ar rcs $(LIB) $(OBJS)

rebuild: clear build

clear:
	@rm -rf $(OBJ_DIR)

clean: clear
	@rm -f $(LIB)

makeObjDir:
	@mkdir -p $(OBJ_DIR)

.PHONY: all build clear rebuild clean makeObjDir