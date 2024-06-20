#include "viewer-app-window.h"
#include "viewer-app.h"
#include <epoxy/gl.h>

enum { X_AXIS, Y_AXIS, Z_AXIS, N_AXES };

struct _ViewerAppWindow {
    GtkApplicationWindow parent_instance;

    GtkAdjustment *x_adjustment;
    GtkAdjustment *y_adjustment;
    GtkAdjustment *z_adjustment;

    // Rotations angles
    double rotation_angles[N_AXES];

      /* model-view-projection matrix */
    float mvp[16];

    GtkWidget *gl_drawing_area;
};

struct _ViewerAppWindowClass
{
  GtkApplicationWindowClass parent_class;
};

G_DEFINE_TYPE (ViewerAppWindow, viewer_app_window, GTK_TYPE_APPLICATION_WINDOW)

static void
gl_init (ViewerAppWindow *self)
{
  /* we need to ensure that the GdkGLContext is set before calling GL API */
  gtk_gl_area_make_current (GTK_GL_AREA (self->gl_drawing_area));

  /* if the GtkGLArea is in an error state we don't do anything */
  if (gtk_gl_area_get_error (GTK_GL_AREA (self->gl_drawing_area)) != NULL)
    return;

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

static void
gl_fini (ViewerAppWindow *self)
{
  /* we need to ensure that the GdkGLContext is set before calling GL API */
  gtk_gl_area_make_current (GTK_GL_AREA (self->gl_drawing_area));

  /* skip everything if we're in error state */
  if (gtk_gl_area_get_error (GTK_GL_AREA (self->gl_drawing_area)) != NULL)
    return;

  /* destroy all the resources we created */
//   if (self->vao != 0)
//     glDeleteVertexArrays (1, &self->vao);
//   if (self->program != 0)
//     glDeleteProgram (self->program);
}

static void
viewer_app_window_class_init (ViewerAppWindowClass *klass)
{
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  gtk_widget_class_set_template_from_resource(widget_class, "/school21/gdy/_3dviewer/viewer-app-window.ui");

  gtk_widget_class_bind_template_child (widget_class, ViewerAppWindow, gl_drawing_area);
  gtk_widget_class_bind_template_child (widget_class, ViewerAppWindow, x_adjustment);
  gtk_widget_class_bind_template_child (widget_class, ViewerAppWindow, y_adjustment);
  gtk_widget_class_bind_template_child (widget_class, ViewerAppWindow, z_adjustment);

//   gtk_widget_class_bind_template_callback (widget_class, adjustment_changed);
  gtk_widget_class_bind_template_callback (widget_class, gl_init);
  gtk_widget_class_bind_template_callback (widget_class, gl_fini);

}

static void
viewer_app_window_init (ViewerAppWindow *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));

  /* reset the mvp matrix */
//   init_mvp (self->mvp);

//   gtk_window_set_icon_name (GTK_WINDOW (self), "viewer");
}

GtkWidget *
viewer_app_window_new (ViewerApp *app)
{
  return g_object_new (viewer_app_window_get_type (), "application", app, NULL);
}
