#ifndef _VIEWER_SETLABEL_H
#define _VIEWER_SETLABEL_H

#include <epoxy/gl.h>
#include <gtk/gtk.h>

#include "parser.h"

typedef struct {
  GtkLabel *model_name_label;
  ObjFile_t *obj_file;
} AppData;

const char *format_number(int number);

void update_label_text(AppData *self);
#endif  // _VIEWER_SETLABEL_H
