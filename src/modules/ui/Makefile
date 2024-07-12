CC ?= gcc
PKGCONFIG = $(shell which pkg-config)
CFLAGS = $(shell $(PKGCONFIG) --cflags gtk4 epoxy cglm gdk-pixbuf-2.0 cgif) -Wno-overlength-strings -Wall -Wextra -pedantic -std=c11 -O3 -g
LIBS = $(shell $(PKGCONFIG) --libs gtk4 epoxy cglm gdk-pixbuf-2.0 cgif)
GLIB_COMPILE_RESOURCES = $(shell $(PKGCONFIG) --variable=glib_compile_resources gio-2.0)
GLIB_COMPILE_SCHEMAS = $(shell $(PKGCONFIG) --variable=glib_compile_schemas gio-2.0)

SRC = viewer-app.c viewer-app-window.c viewer-error.c viewer-app-settings.c viewer-setLabel.c
BUILT_SRC = resources.c

OBJS = $(BUILT_SRC:.c=.o) $(SRC:.c=.o)

all: $(OBJS) gschemas.compiled

school21.gdy._3dviewer.gschema.valid: school21.gdy._3dviewer.gschema.xml
	$(GLIB_COMPILE_SCHEMAS) --strict --dry-run --schema-file=$< && mkdir -p $(@D) && touch $@

gschemas.compiled: school21.gdy._3dviewer.gschema.valid
	$(GLIB_COMPILE_SCHEMAS) .

resources.c: viewer-app.gresource.xml viewer-app-window.ui viewer-app-menu.ui viewer-app-settings.ui
	@$(GLIB_COMPILE_RESOURCES) viewer-app.gresource.xml --target=$@ --sourcedir=. --generate-source

%.o: %.c
	@$(CC) -c -o $(@F) $(CFLAGS) $<

clean:
	@rm -f school21.gdy._3dviewer.gschema.valid
	@rm -f gschemas.compiled
	@rm -f $(BUILT_SRC)
	@rm -f $(OBJS)
	@rm -f resources.c