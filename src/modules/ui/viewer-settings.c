#include "viewer-settings.h"
#include "viewer-app-settings.h"

void open_settings_dialog(GAction *action, GVariant *parameter,
                                 ViewerAppWindow *self) {
  g_print("Opening settings dialog\n");
  ViewerAppSettings *settings_dialog = viewer_app_settings_new(self);
  gtk_window_present(GTK_WINDOW(settings_dialog));
}

void set_orthographic_projection(float left, float right, float bottom,
                                 float top, float near, float far) {
  // glMatrixMode(GL_PROJECTION);
  // glLoadIdentity();
  // glOrtho(left, right, bottom, top, near, far);
  // glMatrixMode(GL_MODELVIEW);
}

void set_perspective_projection(float left, float right, float bottom,
                                float top, float near, float far) {
  // glMatrixMode(GL_PROJECTION);
  // glLoadIdentity();
  // glFrustum(left, right, bottom, top, near, far);
  // glMatrixMode(GL_MODELVIEW);
}

void set_edge_thickness(float thickness) {
  GLfloat lineWidthRange[2] = {0.0f, 0.0f};
  glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, lineWidthRange);
  g_print("Setting edge thickness to %f\n", thickness);
  glLineWidth(thickness / 10.f);
}

void set_edge_strippled(ViewerAppWindow *self) {
  self->pattern = 0x18ff;
  self->factor = 2.f;
} 

void set_edge_not_strippled(ViewerAppWindow *self) {
  self->pattern = 0xffff;
  self->factor = 1.f;
}

void apply_projection_type_setting(ViewerAppWindow *self) {
  const gchar *projection_type = g_settings_get_string(self->settings, "projection-type");
  if (g_strcmp0(projection_type, "Parallel") == 0) {
    set_orthographic_projection(-1.0, 1.0, -1.0, 1.0, 5, 100);
  } else if (g_strcmp0(projection_type, "Central") == 0) {
    set_perspective_projection(-1.0, 1.0, -1.0, 1.0, 5, 100);
  }
}

void apply_edge_type_setting(ViewerAppWindow *self) {
  const gchar *edge_type = g_settings_get_string(self->settings, "edge-type");
  if (g_strcmp0(edge_type, "Solid") == 0) {
    set_edge_not_strippled(self);
  } else if (g_strcmp0(edge_type, "Dashed") == 0) {
    set_edge_strippled(self);
  }
}


void apply_edge_thickness_setting(ViewerAppWindow *self) {
  set_edge_thickness(g_settings_get_int(self->settings, "edge-thickness"));
}

void apply_edge_color_setting(ViewerAppWindow *self) {
  GVariant *color_variant = g_settings_get_value(self->settings, "edge-color");
  g_variant_get(color_variant, "(iiii)", &self->edge_color[0], &self->edge_color[1], &self->edge_color[2], NULL);
  g_variant_unref(color_variant);
}

void apply_background_color_setting(ViewerAppWindow *self) {
  GVariant *color_variant = g_settings_get_value(self->settings, "background-color");
  g_variant_get(color_variant, "(iiii)", &self->background_color.red, &self->background_color.green, &self->background_color.blue, &self->background_color.alpha);
  g_variant_unref(color_variant);
}

void on_settings_changed(GSettings *settings, gchar *key,
                                ViewerAppWindow *self) {
  if (g_strcmp0(key, "projection-type") == 0) {
    apply_projection_type_setting(self);
    gtk_widget_queue_draw(self->gl_drawing_area);
  }

  if (g_strcmp0(key, "edge-type") == 0) {
    apply_edge_type_setting(self);
    gtk_widget_queue_draw(self->gl_drawing_area);
  }

  if (g_strcmp0(key, "edge-thickness") == 0) {
    apply_edge_thickness_setting(self);
    gtk_widget_queue_draw(self->gl_drawing_area);
  }

  if (g_strcmp0(key, "edge-color") == 0) {
    apply_edge_color_setting(self);
    gtk_widget_queue_draw(self->gl_drawing_area);
  }

  if (g_strcmp0(key, "background-color") == 0) {
    apply_background_color_setting(self);
    gtk_widget_queue_draw(self->gl_drawing_area);
  }
}