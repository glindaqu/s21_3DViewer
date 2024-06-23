#include "viewer-app-window.h"

#include <epoxy/gl.h>

#include "viewer-app.h"

enum { X_AXIS, Y_AXIS, Z_AXIS, N_AXES };

struct _ViewerAppWindow {
  GtkApplicationWindow parent_instance;

  GtkHeaderBar *header_bar;

  GtkAdjustment *x_adjustment;
  GtkAdjustment *y_adjustment;
  GtkAdjustment *z_adjustment;

  GtkWidget *primary_menu;
  GtkButton *open_button;
  // Rotations angles
  double rotation_angles[N_AXES];

  /* model-view-projection matrix */
  float mvp[16];

  GtkWidget *gl_drawing_area;

  /* GL objects */
  guint vao;
  guint program;
  guint mvp_location;
  guint position_index;
  guint color_index;
};

struct _ViewerAppWindowClass {
  GtkApplicationWindowClass parent_class;
};

G_DEFINE_TYPE(ViewerAppWindow, viewer_app_window, GTK_TYPE_APPLICATION_WINDOW)

static void open_file_complete(GObject *source_object, GAsyncResult *result,
                               ViewerAppWindow *self) {
  GFile *file = G_FILE(source_object);

  g_autofree char *contents = NULL;
  gsize length = 0;

  g_autoptr(GError) error = NULL;

  // Complete the asynchronous operation; this function will either
  // give you the contents of the file as a byte array, or will
  // set the error argument
  g_file_load_contents_finish(file, result, &contents, &length, NULL, &error);

  if (error != NULL) {
    g_printerr("Unable to open “%s”: %s\n", g_file_peek_path(file),
               error->message);
    return;
  }
  if (!g_utf8_validate(contents, length, NULL)) {
    g_printerr(
        "Unable to load the contents of “%s”: "
        "the file is not encoded with UTF-8\n",
        g_file_peek_path(file));
    return;
  }
}

static void open_file(ViewerAppWindow *self, GFile *file) {
  g_file_load_contents_async(file, NULL,
                             (GAsyncReadyCallback)open_file_complete, self);
}

static void on_open_response(GObject *source, GAsyncResult *result,
                             gpointer user_data) {
  GtkFileDialog *dialog = GTK_FILE_DIALOG(source);
  ViewerApp *self = user_data;

  g_autoptr(GFile) file = gtk_file_dialog_open_finish(dialog, result, NULL);

  if (file != NULL) open_file(self, file);
}

static void viewer_app_window__open_file_dialog(GAction *action,
                                                GVariant *parameter,
                                                ViewerAppWindow *self) {
  g_autoptr(GtkFileDialog) dialog = gtk_file_dialog_new();

  gtk_file_dialog_open(dialog, GTK_WINDOW(self), NULL, on_open_response, self);
}

static void gl_init(ViewerAppWindow *self) {
  /* we need to ensure that the GdkGLContext is set before calling GL API */
  gtk_gl_area_make_current(GTK_GL_AREA(self->gl_drawing_area));

  /* if the GtkGLArea is in an error state we don't do anything */
  if (gtk_gl_area_get_error(GTK_GL_AREA(self->gl_drawing_area)) != NULL) return;

  /* initialize the shaders and retrieve the program data */
  //   GError *error = NULL;
  //   if (!init_shaders (&self->program,
  //                      &self->mvp_location,
  //                      &self->position_index,
  //                      &self->color_index,
  //                      &error))
  //     {
  //       /* set the GtkGLArea in error state, so we'll see the error message
  //        * rendered inside the viewport
  //        */
  //       gtk_gl_area_set_error (GTK_GL_AREA (self->gl_drawing_area), error);
  //       g_error_free (error);
  //       return;
  //     }

  /* initialize the vertex buffers */
  //   init_buffers (self->position_index, self->color_index, &self->vao);
}

static void gl_fini(ViewerAppWindow *self) {
  /* we need to ensure that the GdkGLContext is set before calling GL API */
  gtk_gl_area_make_current(GTK_GL_AREA(self->gl_drawing_area));

  /* skip everything if we're in error state */
  if (gtk_gl_area_get_error(GTK_GL_AREA(self->gl_drawing_area)) != NULL) return;

  /* destroy all the resources we created */
  //   if (self->vao != 0)
  //     glDeleteVertexArrays (1, &self->vao);
  //   if (self->program != 0)
  //     glDeleteProgram (self->program);
}

static void viewer_app_window_class_init(ViewerAppWindowClass *klass) {
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);

  gtk_widget_class_set_template_from_resource(
      widget_class,
      "/src/modules/ui/school21/gdy/_3dviewer/viewer-app-window.ui");

  gtk_widget_class_bind_template_child(widget_class, ViewerAppWindow,
                                       header_bar);
  gtk_widget_class_bind_template_child(widget_class, ViewerAppWindow,
                                       open_button);
  gtk_widget_class_bind_template_child(widget_class, ViewerAppWindow, primary_menu);                                     
  gtk_widget_class_bind_template_child(widget_class, ViewerAppWindow,
                                       gl_drawing_area);
  gtk_widget_class_bind_template_child(widget_class, ViewerAppWindow,
                                       x_adjustment);
  gtk_widget_class_bind_template_child(widget_class, ViewerAppWindow,
                                       y_adjustment);
  gtk_widget_class_bind_template_child(widget_class, ViewerAppWindow,
                                       z_adjustment);

  //   gtk_widget_class_bind_template_callback (widget_class,
  //   adjustment_changed);
  gtk_widget_class_bind_template_callback(widget_class, gl_init);
  gtk_widget_class_bind_template_callback(widget_class, gl_fini);
}

static void viewer_app_window_init(ViewerAppWindow *self) {
  GtkBuilder *builder;
  GMenuModel *menu;
  GAction *action;

  gtk_widget_init_template(GTK_WIDGET(self));
  
  builder = gtk_builder_new_from_resource ("/src/modules/ui/school21/gdy/_3dviewer/viewer-app-menu.ui");
  menu = G_MENU_MODEL (gtk_builder_get_object (builder, "appmenu"));
  gtk_menu_button_set_menu_model (GTK_MENU_BUTTON (self->primary_menu), menu);
  g_object_unref (builder);

  g_autoptr(GSimpleAction) open_action = g_simple_action_new("open", NULL);
  g_signal_connect(open_action, "activate",
                   G_CALLBACK(viewer_app_window__open_file_dialog), self);
  g_action_map_add_action(G_ACTION_MAP(self), G_ACTION(open_action));
  /* reset the mvp matrix */
  //   init_mvp (self->mvp);

  //   gtk_window_set_icon_name (GTK_WINDOW (self), "viewer");
}

// static void
// viewer_app_window_dispose (GObject *object)
// {
  // ViewerAppWindow *win;

  // win = VIEWER_APP_WINDOW (object);
// 
  // g_clear_object (&win->settings);

  // G_OBJECT_CLASS (vie)->dispose (object);
// }

GtkWidget *viewer_app_window_new(ViewerApp *app) {
  return g_object_new(viewer_app_window_get_type(), "application", app, NULL);
}
