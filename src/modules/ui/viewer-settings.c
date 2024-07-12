#include "../../include/viewer-settings.h"
#include "../../include/viewer-app-settings.h"
#include <cglm/cglm.h>

void open_settings_dialog(GAction *action, GVariant *parameter,
                                 ViewerAppWindow *self) {
  g_print("Opening settings dialog\n");
  ViewerAppSettings *settings_dialog = viewer_app_settings_new(self);
  gtk_window_present(GTK_WINDOW(settings_dialog));
}

void set_perspective_projection(ViewerAppWindow* self, float fovy, float aspect, float nearZ, float farZ) {
  glm_perspective(fovy, aspect, nearZ, farZ, self->projection_matrix);
}

void set_ortho_projection(ViewerAppWindow* self, float left, float right, float bottom,
                                float top, float nearZ, float farZ) {
  glm_ortho(left, right, bottom, top, nearZ, farZ, self->projection_matrix);
}

void set_edge_thickness(ViewerAppWindow* self, float thickness) {
    self->edge_thickness = thickness;
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
    float fovy = glm_rad(90.0f);
    float aspect = 16.0f / 9.0f;
    float nearZ = 0.f;
    float farZ = 10.0f;
    set_perspective_projection(self, fovy, aspect, nearZ, farZ);
  } else if (g_strcmp0(projection_type, "Central") == 0) {
    float left = -1.0f; 
    float right = 1.0f; 
    float bottom = -1.0f; 
    float top = 1.0f; 
    float nearZ = -1.0f;
    float farZ = 1.0f;
    set_ortho_projection(self, left, right, bottom, top, nearZ, farZ);
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
  set_edge_thickness(self, g_settings_get_int(self->settings, "edge-thickness"));
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

void apply_point_color_setting(ViewerAppWindow *self) {
  GVariant *color_variant = g_settings_get_value(self->settings, "point-color");
  g_variant_get(color_variant, "(iiii)", &self->point_color[0], &self->point_color[1], &self->point_color[2], NULL);
  g_variant_unref(color_variant);
}

void apply_point_size_setting(ViewerAppWindow *self) {
  self->point_size = g_settings_get_int(self->settings, "point-size");
}

void apply_point_type_setting(ViewerAppWindow *self) {
  const gchar *point_type = g_settings_get_string(self->settings, "point-type");
  if (g_strcmp0(point_type, "None") == 0) {
    self->point_type = 0;
  } else if (g_strcmp0(point_type, "Circle") == 0) {
    self->point_type = 1;
  }
  else if (g_strcmp0(point_type, "Square") == 0) {
    self->point_type = 2;
  }
}

void on_settings_changed(GSettings *settings, gchar *key,
                                ViewerAppWindow *self) {
  if (g_strcmp0(key, "projection-type") == 0) {
    apply_projection_type_setting(self);
    gtk_widget_queue_draw(self->gl_drawing_area);
  }

  if (g_strcmp0(key, "point-size") == 0) {
    apply_point_size_setting(self);
    gtk_widget_queue_draw(self->gl_drawing_area);
  }

  if (g_strcmp0(key, "point-type") == 0) {
    apply_point_type_setting(self);
    gtk_widget_queue_draw(self->gl_drawing_area);
  }

  if (g_strcmp0(key, "point-color") == 0) {
    apply_point_color_setting(self);
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
