#include "viewer-app-settings.h"

#include <gtk/gtk.h>

struct _ViewerAppSettings {
  GtkDialog parent;

  GSettings *settings;

  GtkWidget *projection;
  GtkWidget *edge_type;
  GtkWidget *edge_color;
  GtkWidget *edge_thickness_spin;
  GtkWidget *vertex_display;
  GtkWidget *vertex_color;
  GtkWidget *vertex_size;
  GtkWidget *background_color;
};

G_DEFINE_TYPE(ViewerAppSettings, viewer_app_settings, GTK_TYPE_DIALOG)

static gboolean rgba_to_gvalue(GValue *value, GVariant *variant, gpointer user_data) {
    gint rgba[4];
    g_variant_get(variant, "(iiii)", &rgba[0], &rgba[1], &rgba[2], &rgba[3]);
    rgba[0] /= 255.f;
    rgba[1] /= 255.f;
    rgba[2] /= 255.f; 
    g_value_set_boxed(value, rgba);
    return TRUE;
}

static GVariant *gvalue_to_rgba(const GValue *value, const GVariantType *expected_type, gpointer user_data) {
    gint *rgba = g_value_get_boxed(value);
    return g_variant_new("(iiii)", rgba[0], rgba[1], rgba[2], rgba[3]);
}


static gboolean projection_to_pos(GValue *value, GVariant *variant,
                                  gpointer user_data) {
  const char *s = g_variant_get_string(variant, NULL);
  if (strcmp(s, "Parallel") == 0)
    g_value_set_uint(value, 0);
  else
    g_value_set_uint(value, 1);

  return TRUE;
}

static GVariant *pos_to_projection(const GValue *value,
                                   const GVariantType *expected_type,
                                   gpointer user_data) {
  switch (g_value_get_uint(value)) {
    case 0:
      return g_variant_new_string("Parallel");
    case 1:
      return g_variant_new_string("Central");
    default:
      g_assert_not_reached();
  }
}

static gboolean edge_type_to_pos(GValue *value, GVariant *variant,
                                 gpointer user_data) {
  const char *s = g_variant_get_string(variant, NULL);
  if (strcmp(s, "Solid") == 0)
    g_value_set_uint(value, 0);
  else
    g_value_set_uint(value, 1);

  return TRUE;
}

static GVariant *pos_to_edge_type(const GValue *value,
                                  const GVariantType *expected_type,
                                  gpointer user_data) {
  switch (g_value_get_uint(value)) {
    case 0:
      return g_variant_new_string("Solid");
    case 1:
      return g_variant_new_string("Dashed");
    default:
      g_assert_not_reached();
  }
}

static void viewer_app_settings_init(ViewerAppSettings *self) {
    gtk_widget_init_template(GTK_WIDGET(self));

    const char cssPath[] =
        "/src/modules/ui/school21/gdy/_3dviewer/appsettings-style.css";
    GtkCssProvider *cssProvider = gtk_css_provider_new();
    gtk_css_provider_load_from_resource(cssProvider, cssPath);
    gtk_style_context_add_provider_for_display(gdk_display_get_default(),
                                               GTK_STYLE_PROVIDER(cssProvider),
                                               GTK_STYLE_PROVIDER_PRIORITY_USER);

    self->settings = g_settings_new("school21.gdy._3dviewer");

    g_object_unref(cssProvider);

    g_settings_bind_with_mapping(self->settings, "projection-type",
                                 self->projection, "selected",
                                 G_SETTINGS_BIND_DEFAULT, projection_to_pos,
                                 pos_to_projection, NULL, NULL);
    g_settings_bind_with_mapping(self->settings, "edge-type", self->edge_type,
                                 "selected", G_SETTINGS_BIND_DEFAULT,
                                 edge_type_to_pos, pos_to_edge_type, NULL, NULL);

    g_settings_bind_with_mapping(self->settings, "background-color", self->background_color, "rgba", G_SETTINGS_BIND_DEFAULT, rgba_to_gvalue, gvalue_to_rgba, NULL, NULL);
    g_settings_bind_with_mapping(self->settings, "edge-color", self->edge_color, "rgba", G_SETTINGS_BIND_DEFAULT, rgba_to_gvalue, gvalue_to_rgba, NULL, NULL);
    // Bind the GtkSpinButton, not the GtkAdjustment
    // g_settings_bind(self->settings, "edge-thickness", self->edge_thickness_spin, "value", G_SETTINGS_BIND_DEFAULT);

    // Bind the GtkColorButton with custom mapping
    // g_settings_bind_with_mapping(self->settings, "line-color", self->line_color_button, "rgba", G_SETTINGS_BIND_DEFAULT, rgba_to_as, as_to_rgba, NULL, NULL);
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

    gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(klass),
                                         ViewerAppSettings, projection);
    gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(klass),
                                         ViewerAppSettings, edge_type);
    gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(klass), ViewerAppSettings, edge_color);
    gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(klass), ViewerAppSettings, background_color);
}

ViewerAppSettings *viewer_app_settings_new(ViewerAppWindow *win) {
  return g_object_new(VIEWER_APP_SETTINGS_TYPE, "transient-for", win,
                      "use-header-bar", TRUE, NULL);
}
