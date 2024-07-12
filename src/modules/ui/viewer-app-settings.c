#include "../../include/viewer-app-settings.h"

#include <gtk/gtk.h>

struct _ViewerAppSettings {
  GtkDialog parent;

  GSettings *settings;

  GtkWidget *projection;
  GtkWidget *edge_type;
  GtkWidget *edge_color;
  GtkWidget *edge_thickness_spin;
  GtkWidget *point_type;
  GtkWidget *point_color;
  GtkWidget *point_size;
  GtkWidget *background_color;
};

G_DEFINE_TYPE(ViewerAppSettings, viewer_app_settings, GTK_TYPE_DIALOG)

static gboolean point_type_to_pos(GValue *value, GVariant *variant,
                                  gpointer user_data) {
  const char *s = g_variant_get_string(variant, NULL);
  if (strcmp(s, "None") == 0)
    g_value_set_uint(value, 0);
  else if (strcmp(s, "Circle") == 0)
    g_value_set_uint(value, 1);
  else
    g_value_set_uint(value, 2);

  return TRUE;
}

static GVariant *pos_to_point_type(const GValue *value,
                                   const GVariantType *expected_type,
                                   gpointer user_data) {
  switch (g_value_get_uint(value)) {
    case 0:
      return g_variant_new_string("None");
    case 1: 
      return g_variant_new_string("Circle");
    case 2:
      return g_variant_new_string("Square");
    default:
      g_assert_not_reached();
  }
}


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
    self->settings = g_settings_new("school21.gdy._3dviewer");

    g_settings_bind_with_mapping(self->settings, "projection-type",
                                 self->projection, "selected",
                                 G_SETTINGS_BIND_DEFAULT, projection_to_pos,
                                 pos_to_projection, NULL, NULL);
    g_settings_bind_with_mapping(self->settings, "edge-type", self->edge_type,
                                 "selected", G_SETTINGS_BIND_DEFAULT,
                                 edge_type_to_pos, pos_to_edge_type, NULL, NULL);
    g_settings_bind_with_mapping(self->settings, "point-type", self->point_type,
                                 "selected", G_SETTINGS_BIND_DEFAULT,
                                 point_type_to_pos, pos_to_point_type, NULL, NULL);
    g_settings_bind_with_mapping(self->settings, "background-color", self->background_color, "rgba", G_SETTINGS_BIND_DEFAULT, rgba_to_gvalue, gvalue_to_rgba, NULL, NULL);
    g_settings_bind_with_mapping(self->settings, "edge-color", self->edge_color, "rgba", G_SETTINGS_BIND_DEFAULT, rgba_to_gvalue, gvalue_to_rgba, NULL, NULL);
    g_settings_bind_with_mapping(self->settings, "point-color", self->point_color, "rgba", G_SETTINGS_BIND_DEFAULT, rgba_to_gvalue, gvalue_to_rgba, NULL, NULL);
    g_settings_bind(self->settings, "edge-thickness", self->edge_thickness_spin, "value", G_SETTINGS_BIND_DEFAULT);
    g_settings_bind(self->settings, "point-size", self->point_size, "value", G_SETTINGS_BIND_DEFAULT);
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
    gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(klass), ViewerAppSettings, point_size);
    gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(klass), ViewerAppSettings, point_type);
    gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(klass), ViewerAppSettings, point_color);
    gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(klass), ViewerAppSettings, edge_color);
    gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(klass), ViewerAppSettings, background_color);
    gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(klass), ViewerAppSettings, edge_thickness_spin);
}

ViewerAppSettings *viewer_app_settings_new(ViewerAppWindow *win) {
  return g_object_new(VIEWER_APP_SETTINGS_TYPE, "transient-for", win,
                      "use-header-bar", TRUE, NULL);
}
