#include "viewer-app.h"

#include <gtk/gtk.h>

#include "viewer-app-window.h"

struct _ViewerApp {
  GtkApplication parent_instance;

  GtkWidget *window;
};

struct _ViewerAppClass {
  GtkApplicationClass parent_class;
};

G_DEFINE_TYPE(ViewerApp, viewer_app, GTK_TYPE_APPLICATION)

static void viewer_app_init(ViewerApp *self) {
  GApplication *app = G_APPLICATION(self);
  self->window = NULL;
  g_object_set(app, "application-id", "school21.gdy._3dviewer", NULL);
}

static void viewer_app_startup(GApplication *app) {
  G_APPLICATION_CLASS(viewer_app_parent_class)->startup(app);
}

static void viewer_app_activate(GApplication *app) {
  ViewerApp *self = VIEWER_APP(app);

  if (self->window == NULL) {
    printf("self->window = viewer_app_window_new (app);\n");
    self->window = viewer_app_window_new(VIEWER_APP(app));
  }

  gtk_window_present(GTK_WINDOW(self->window));
}

static void viewer_app_class_init(ViewerAppClass *klass) {
  GApplicationClass *app_class = G_APPLICATION_CLASS(klass);

  app_class->startup = viewer_app_startup;
  app_class->activate = viewer_app_activate;
}

GtkApplication *viewer_app_new(void) {
  return g_object_new(viewer_app_get_type(), "application-id",
                      "school21.gdy._3dviewer", NULL);
}
