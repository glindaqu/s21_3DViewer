#include "viewer-app-window.h"
#include "viewer-app.h"

enum { X_AXIS, Y_AXIS, Z_AXIS, N_AXES };

struct _ViewerAppWindow {
    GtkApplicationWindow parent_instance;

    // GtkAdjustment *x_adjustment;
    // GtkAdjustment *y_adjustment;
    // GtkAdjustment *z_adjustment;

    // GtkWidget *gl_drawing_area;
};

struct _ViewerAppWindowClass
{
  GtkApplicationWindowClass parent_class;
};

G_DEFINE_TYPE (ViewerAppWindow, viewer_app_window, GTK_TYPE_APPLICATION_WINDOW)

static void
viewer_app_window_class_init (ViewerAppWindowClass *klass)
{
//   GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

//   gtk_widget_class_set_template_from_resource(widget_class, "/school21/gdy/_3dviewer/viewer-app-window.ui");
}

static void
viewer_app_window_init (ViewerAppWindow *self)
{
//   gtk_widget_init_template (GTK_WIDGET (self));

  /* reset the mvp matrix */
//   init_mvp (self->mvp);

//   gtk_window_set_icon_name (GTK_WINDOW (self), "viewer");
}

GtkWidget *
viewer_app_window_new (ViewerApp *app)
{
  return g_object_new (viewer_app_window_get_type (), "application", app, NULL);
}
