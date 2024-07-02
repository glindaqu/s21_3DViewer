CC ?= gcc
PKGCONFIG = $(shell which pkg-config)
CFLAGS = $(shell $(PKGCONFIG) --cflags gtk4 epoxy) -Wall -Wextra -pedantic -std=c11 -O3 -g
LIBS = $(shell $(PKGCONFIG) --libs gtk4 epoxy)
GLIB_COMPILE_RESOURCES = $(shell $(PKGCONFIG) --variable=glib_compile_resources gio-2.0)

SRC = viewer-app.c viewer-app-window.c viewer-error.c viewer-glfuncs.c
BUILT_SRC = resources.c

OBJS = $(BUILT_SRC:.c=.o) $(SRC:.c=.o)

all: $(OBJS)

resources.c: viewer-app.gresource.xml viewer-app-window.ui viewer-app-menu.ui
	@$(GLIB_COMPILE_RESOURCES) viewer-app.gresource.xml --target=$@ --sourcedir=. --generate-source

%.o: %.c
	@$(CC) -c -o $(@F) $(CFLAGS) $<

clean:
	@rm -f $(BUILT_SRC)
	@rm -f $(OBJS)
	@rm -f resources.c