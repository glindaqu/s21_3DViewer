#include "viewer-app-settings.h"

#include <gtk/gtk.h>

struct _ViewerAppSettings {
  GtkDialog parent;

  GSettings *settings;
};

G_DEFINE_TYPE(ViewerAppSettings, viewer_app_settings, GTK_TYPE_DIALOG)

static void viewer_app_settings_init(ViewerAppSettings *self) {
  gtk_widget_init_template(GTK_WIDGET(self));
  self->settings = g_settings_new("school21.gdy._3dviewer");
}

static void viewer_app_settings_dispose(GObject *object) {
  ViewerAppSettings *settings;

  settings = VIEWER_APP_SETTINGS(object);

  g_clear_object(&settings->settings);

  G_OBJECT_CLASS(viewer_app_settings_parent_class)->dispose(object);
}

static void viewer_app_settings_class_init(ViewerAppSettingsClass *klass) {
  G_OBJECT_CLASS(klass)->dispose = viewer_app_settings_dispose;

  gtk_widget_class_set_template_from_resource(
      GTK_WIDGET_CLASS(klass),
      "/src/modules/ui/school21/gdy/_3dviewer/"
      "viewer-app-settings.ui");
  //   gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class),
  //   ExampleAppPrefs, font); gtk_widget_class_bind_template_child
  //   (GTK_WIDGET_CLASS (class), ExampleAppPrefs, transition);
}

ViewerAppSettings *viewer_app_settings_new(ViewerAppWindow *win) {
  return g_object_new(VIEWER_APP_SETTINGS_TYPE, "transient-for", win,
                      "use-header-bar", TRUE, NULL);
}
