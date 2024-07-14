CC ?= gcc
PKGCONFIG = $(shell which pkg-config)
CFLAGS = $(shell $(PKGCONFIG) --cflags gtk4 epoxy cglm gdk-pixbuf-2.0 cgif) -Wno-unused-function -Wall -Wextra -pedantic -std=c11 -O3
LIBS = $(shell $(PKGCONFIG) --libs gtk4 epoxy cglm gdk-pixbuf-2.0 cgif)

SRC = viewer-settings.c

OBJS = $(SRC:.c=.o)

all: $(OBJS)

%.o: %.c
	@$(CC) -c -o $(@F) $(CFLAGS) $<

clean:
	@rm -f $(OBJS)
