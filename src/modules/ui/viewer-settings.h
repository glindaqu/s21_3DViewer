#ifndef _VIEWER_SETTINGS_H
#define _VIEWER_SETTINGS_H

#include <gtk/gtk.h>

#include "viewer-app-window.h"

void open_settings_dialog(GAction *action, GVariant *parameter,
                                 ViewerAppWindow *self);

void set_perspective_projection(ViewerAppWindow* self, float fovy, float aspect, float nearZ, float farZ);

void set_ortho_projection(ViewerAppWindow* self, float left, float right, float bottom,
                                float top, float nearZ, float farZ);

void apply_point_size_setting(ViewerAppWindow *self);

void apply_point_type_setting(ViewerAppWindow *self);

void apply_point_color_setting(ViewerAppWindow *self);

void set_edge_thickness(ViewerAppWindow* self, float thickness);

void set_edge_strippled(ViewerAppWindow *self);

void set_edge_not_strippled(ViewerAppWindow *self);

void apply_projection_type_setting(ViewerAppWindow *self);

void apply_edge_type_setting(ViewerAppWindow *self);

void apply_edge_thickness_setting(ViewerAppWindow *self);

void set_line_color(const GVariant *color_variant, float* lineColor);

void apply_edge_color_setting(ViewerAppWindow *self);

void apply_background_color_setting(ViewerAppWindow *self);
void on_settings_changed(GSettings *settings, gchar *key,
                                ViewerAppWindow *self);
#endif  // _VIEWER_SETTINGS_H
