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

// static void
// preferences_activated (GSimpleAction *action,
//                        GVariant      *parameter,
//                        gpointer       app)
// {
//   ViewerApp *prefs;
//   GtkWindow *win;

//   win = gtk_application_get_active_window (GTK_APPLICATION (app));
//   prefs = example_app_prefs_new (EXAMPLE_APP_WINDOW (win));
//   gtk_window_present (GTK_WINDOW (prefs));
// }
static void quit_activated(GSimpleAction *action, GVariant *parameter,
                           gpointer app) {
  const gchar *action_name = g_action_get_name(G_ACTION(action));
  g_print("Action name: %s\n", action_name);

  if (parameter != NULL) {
    const gchar *param_str = g_variant_get_string(parameter, NULL);
    g_print("Parameter value: %s\n", param_str);
  } else {
    g_print("No parameter provided\n");
  }

  g_application_quit(G_APPLICATION(app));
}

static GActionEntry app_entries[] = {
    // { "preferences", preferences_activated, NULL, NULL, NULL, {0} },
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
