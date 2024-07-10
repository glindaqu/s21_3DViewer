#ifndef _3DVIEWER_APP_WINDOW_H
#define _3DVIEWER_APP_WINDOW_H

#include <gtk/gtk.h>

#include "viewer-app.h"
#include "../../include/matrix_calc.h"
#include "../../include/parser.h"
#include "viewer-glfuncs.h"

G_BEGIN_DECLS

#define VIEWER_APP_WINDOW_TYPE (viewer_app_window_get_type())
G_DECLARE_FINAL_TYPE(ViewerAppWindow, viewer_app_window, VIEWER, APP_WINDOW,
                     GtkApplicationWindow)

GtkWidget *viewer_app_window_new(ViewerApp *app);

struct _ViewerAppWindow {
  GtkApplicationWindow parent_instance;

  GtkHeaderBar *header_bar;
  GSettings *settings;

  mvp_matrix_movement_t matrix_movement;

  GtkWidget *primary_menu;
  GtkButton *open_button;

  ObjFile_t *obj_file;

  gl_matrix *mvp_matrix;

  GtkWidget *gl_drawing_area;

  GtkLabel *model_name_label;

  GtkScale *x_scale;
  GtkScale *y_scale;
  GtkScale *z_scale;

  /* GL objects */

  GLbuffers_t gl_buffers;
  GLshader_vars_t shader_vars;

  gfloat edge_color[3];
  GdkRGBA background_color;

  GLushort pattern;
  GLfloat factor;

  /* Mouse state */
  gboolean alt_key;
  gboolean mouse_dragging;
  double last_mouse_x;
  double last_mouse_y;
};

G_END_DECLS

struct _ViewerAppWindowClass {
  GtkApplicationWindowClass parent_class;
};

#endif  // _3DVIEWER_APP_WINDOW_H
