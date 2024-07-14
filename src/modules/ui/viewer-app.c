#include "../../include/viewer-app.h"

#include <gtk/gtk.h>

#include "../../include/viewer-app-window.h"

struct _ViewerApp {
  GtkApplication parent_instance;
  GtkWidget *window;
};

struct _ViewerAppClass {
  GtkApplicationClass parent_class;
};

G_DEFINE_TYPE(ViewerApp, viewer_app, GTK_TYPE_APPLICATION)

static void quit_activated(UNUSED GSimpleAction *action,
                           UNUSED GVariant *parameter, gpointer app) {
  ViewerApp *self = VIEWER_APP(app);
  g_print("Quitting application\n");

  if (self->window) {
    g_print("Disposing window\n");
    g_object_run_dispose(G_OBJECT(self->window));
  }

  g_application_quit(G_APPLICATION(app));
}

static GActionEntry app_entries[] = {
    {"quit", quit_activated, NULL, NULL, NULL, {0}}};

static void viewer_app_init(ViewerApp *self) {
  GApplication *app = G_APPLICATION(self);
  self->window = NULL;
  g_object_set(app, "application-id", "school21.gdy._3dviewer", NULL);
}

static const struct {
  const char *action_name;
  const char **accels;
} app_actions[] = {
    {"app.quit", (const char *[]){"<Ctrl>Q", NULL}},
    {"win.open", (const char *[]){"<Ctrl>O", NULL}},
};

static void viewer_app_startup(GApplication *app) {
  G_APPLICATION_CLASS(viewer_app_parent_class)->startup(app);

  for (size_t i = 0; i < G_N_ELEMENTS(app_actions); i++) {
    gtk_application_set_accels_for_action(GTK_APPLICATION(app),
                                          app_actions[i].action_name,
                                          app_actions[i].accels);
  }

  g_action_map_add_action_entries(G_ACTION_MAP(app), app_entries,
                                  G_N_ELEMENTS(app_entries), app);
}

static void on_window_destroy(gpointer data) {
  g_print("Window destroyed\n");
  GApplication *app = G_APPLICATION(data);
  if (G_IS_APPLICATION(app)) {
    g_print("Quitting application\n");
    g_application_quit(app);
  }
}

static void viewer_app_activate(GApplication *app) {
  ViewerApp *self = VIEWER_APP(app);

  if (self->window == NULL) {
    g_print("Creating new window\n");
    self->window = viewer_app_window_new(VIEWER_APP(app));
    if (self->window == NULL) {
      g_error("Failed to create window\n");
      return;
    }
    g_signal_connect(self->window, "destroy", G_CALLBACK(on_window_destroy),
                     app);
  }

  g_print("Presenting window\n");
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
