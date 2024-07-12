#include "../../include/viewer-app-window.h"

#include <epoxy/gl.h>

#include "../../include/matrix_calc.h"
#include "../../include/parser.h"
#include "../../include/viewer-gif.h"
#include "../../include/viewer-glarea.h"
#include "../../include/viewer-modelMovement.h"
#include "../../include/viewer-openFile.h"
#include "../../include/viewer-screenshots.h"
#include "../../include/viewer-settings.h"

G_DEFINE_TYPE(ViewerAppWindow, viewer_app_window, GTK_TYPE_APPLICATION_WINDOW)

static void show_about_dialog(UNUSED GVariant *parameter, gpointer user_data) {
  ViewerAppWindow *self = VIEWER_APP_WINDOW(user_data);
  GtkWidget *dialog = gtk_about_dialog_new();

  gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(dialog), "3D Viewer");
  gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog), "Version: 1.0");
  gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog),
                                "A simple 3D model viewer application.");
  gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(dialog),
                               "https://github.com/glindaqu/s21_3DViewer");
  gtk_about_dialog_set_website_label(GTK_ABOUT_DIALOG(dialog),
                                     "Visit github for source code");
  gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(dialog),
                                 "Copyright (C) 2024");
  const char *authors[] = {"Glindaqu", "Dipoolat", "Yukikoqo", NULL};
  gtk_about_dialog_set_authors(GTK_ABOUT_DIALOG(dialog), authors);

  GBytes *bytes = g_resources_lookup_data(
      "/src/modules/ui/school21/gdy/_3dviewer/resources/icons/icon.png", 0,
      NULL);
  if (bytes == NULL) {
    g_warning("Failed to load icon");
    return;
  }

  GInputStream *stream = g_memory_input_stream_new_from_bytes(bytes);
  GdkPixbuf *logo = gdk_pixbuf_new_from_stream(stream, NULL, NULL);
  g_object_unref(stream);
  g_bytes_unref(bytes);

  if (logo == NULL) {
    g_warning("Failed to create pixbuf from stream");
    return;
  }

  GdkTexture *texture = gdk_texture_new_for_pixbuf(logo);
  GdkPaintable *paintable_logo = GDK_PAINTABLE(texture);
  gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(dialog), paintable_logo);

  gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(self));
  gtk_widget_set_visible(dialog, TRUE);
}

static void viewer_app_window_dispose(GObject *object) {
  ViewerAppWindow *win = VIEWER_APP_WINDOW(object);

  g_print("Disposing ViewerAppWindow\n");

  g_clear_object(&win->settings);

  free_frame_buffer();

  if (win->mvp_matrix) {
    g_print("Freeing mvp_matrix\n");
    free(win->mvp_matrix);
    win->mvp_matrix = NULL;
  }

  if (win->obj_file) {
    g_print("Removing and freeing obj_file\n");
    removeObjFile(win->obj_file);
    free(win->obj_file);
    win->obj_file = NULL;
  }

  G_OBJECT_CLASS(viewer_app_window_parent_class)->dispose(object);
}

static void viewer_app_window_add_childs(GtkWidgetClass *widget_class) {
  gtk_widget_class_bind_template_child(widget_class, ViewerAppWindow,
                                       header_bar);
  gtk_widget_class_bind_template_child(widget_class, ViewerAppWindow,
                                       open_button);
  gtk_widget_class_bind_template_child(widget_class, ViewerAppWindow,
                                       primary_menu);
  gtk_widget_class_bind_template_child(widget_class, ViewerAppWindow,
                                       gl_drawing_area);

  gtk_widget_class_bind_template_child(widget_class, ViewerAppWindow, x_scale);
  gtk_widget_class_bind_template_child(widget_class, ViewerAppWindow, y_scale);
  gtk_widget_class_bind_template_child(widget_class, ViewerAppWindow, z_scale);
  gtk_widget_class_bind_template_child(widget_class, ViewerAppWindow,
                                       model_name_label);

  gtk_widget_class_bind_template_callback(widget_class, gl_init);
  gtk_widget_class_bind_template_callback(widget_class, gl_draw);
  gtk_widget_class_bind_template_callback(widget_class, gl_fini);
}

static void viewer_app_window_class_init(ViewerAppWindowClass *klass) {
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);
  G_OBJECT_CLASS(klass)->dispose = viewer_app_window_dispose;

  gtk_widget_class_set_template_from_resource(
      widget_class,
      "/src/modules/ui/school21/gdy/_3dviewer/viewer-app-window.ui");

  viewer_app_window_add_childs(widget_class);
}

static void viewer_app_window_add_css_style(void) {
  const char cssPath[] =
      "/src/modules/ui/school21/gdy/_3dviewer/resources/css/window-style.css";
  GtkCssProvider *cssProvider = gtk_css_provider_new();
  gtk_css_provider_load_from_resource(cssProvider, cssPath);
  gtk_style_context_add_provider_for_display(gdk_display_get_default(),
                                             GTK_STYLE_PROVIDER(cssProvider),
                                             GTK_STYLE_PROVIDER_PRIORITY_USER);

  g_object_unref(cssProvider);
}

static void viewer_app_window_add_settings(ViewerAppWindow *self) {
  g_autoptr(GSimpleAction) settings_action =
      g_simple_action_new("settings", NULL);
  g_signal_connect(settings_action, "activate",
                   G_CALLBACK(open_settings_dialog), self);
  g_action_map_add_action(G_ACTION_MAP(self), G_ACTION(settings_action));

  self->settings = g_settings_new("school21.gdy._3dviewer");
  g_signal_connect(self->settings, "changed", G_CALLBACK(on_settings_changed),
                   self);
}

static void viewer_app_window_add_open_action(ViewerAppWindow *self) {
  g_autoptr(GSimpleAction) open_action = g_simple_action_new("open", NULL);
  g_signal_connect(open_action, "activate",
                   G_CALLBACK(viewer_app_window__open_file_dialog), self);
  g_action_map_add_action(G_ACTION_MAP(self), G_ACTION(open_action));
}

static void viewer_app_window_load_ui(ViewerAppWindow *self) {
  GtkBuilder *builder = gtk_builder_new_from_resource(
      "/src/modules/ui/school21/gdy/_3dviewer/viewer-app-menu.ui");
  GMenuModel *menu = G_MENU_MODEL(gtk_builder_get_object(builder, "appmenu"));
  gtk_menu_button_set_menu_model(GTK_MENU_BUTTON(self->primary_menu), menu);
  g_object_unref(builder);
}

static void viewer_app_window_load_settings(ViewerAppWindow *self) {
  apply_projection_type_setting(self);
  apply_point_size_setting(self);
  apply_point_type_setting(self);
  apply_point_color_setting(self);
  apply_edge_type_setting(self);
  apply_edge_thickness_setting(self);
  apply_edge_color_setting(self);
  apply_background_color_setting(self);
}

static void viewer_app_window_add_gesture_action(ViewerAppWindow *self) {
  GtkGesture *click_gesture = gtk_gesture_click_new();
  g_signal_connect(click_gesture, "pressed", G_CALLBACK(gl_button_press_event),
                   self);
  g_signal_connect(click_gesture, "released",
                   G_CALLBACK(gl_button_release_event), self);
  gtk_widget_add_controller(GTK_WIDGET(self->gl_drawing_area),
                            GTK_EVENT_CONTROLLER(click_gesture));
}

static void viewer_app_window_add_motion_action(ViewerAppWindow *self) {
  GtkEventController *motion_controller = gtk_event_controller_motion_new();
  g_signal_connect(motion_controller, "motion",
                   G_CALLBACK(gl_motion_notify_event), self);
  gtk_widget_add_controller(GTK_WIDGET(self->gl_drawing_area),
                            motion_controller);
}

static void viewer_app_about_action(ViewerAppWindow *self) {
  g_autoptr(GSimpleAction) about_action = g_simple_action_new("about", NULL);
  g_signal_connect(about_action, "activate", G_CALLBACK(show_about_dialog),
                   self);
  g_action_map_add_action(G_ACTION_MAP(self), G_ACTION(about_action));
}

static void viewer_app_window_add_scroll_action(ViewerAppWindow *self) {
  GtkEventController *scroll_controller =
      gtk_event_controller_scroll_new(GTK_EVENT_CONTROLLER_SCROLL_BOTH_AXES);
  g_signal_connect(scroll_controller, "scroll", G_CALLBACK(gl_scroll_event),
                   self);
  gtk_widget_add_controller(GTK_WIDGET(self->gl_drawing_area),
                            scroll_controller);
}

static void viewer_app_window_add_scale_action(ViewerAppWindow *self) {
  g_signal_connect(self->x_scale, "value-changed",
                   G_CALLBACK(on_scale_value_changed), self);
  g_signal_connect(self->y_scale, "value-changed",
                   G_CALLBACK(on_scale_value_changed), self);
  g_signal_connect(self->z_scale, "value-changed",
                   G_CALLBACK(on_scale_value_changed), self);
}

static void viewer_app_window_init(ViewerAppWindow *self) {
  self->obj_file = calloc(1, sizeof(ObjFile_t));
  self->mvp_matrix = calloc(1, sizeof(gl_matrix));
  initParser(self->obj_file);
  init_mvp_matrix(self->mvp_matrix);
  self->recording = FALSE;
  self->frame_counter = 0;
  self->gif = NULL;

  memset(&self->matrix_movement, 0, sizeof(mvp_matrix_movement_t));

  self->matrix_movement.scale_vector[X_AXIS] = 1.0;
  self->matrix_movement.scale_vector[Y_AXIS] = 1.0;
  self->matrix_movement.scale_vector[Z_AXIS] = 1.0;

  gtk_widget_init_template(GTK_WIDGET(self));

  gtk_gl_area_set_required_version(GTK_GL_AREA(self->gl_drawing_area), 4, 6);

  GtkSettings *settings = gtk_settings_get_default();
  g_object_set(settings, "gtk-theme-name", "Adwaita-dark", NULL);

  viewer_app_window_add_css_style();

  viewer_app_window_load_ui(self);

  viewer_app_window_add_open_action(self);

  viewer_app_window_add_settings(self);

  viewer_app_window_load_settings(self);

  viewer_app_about_action(self);

  viewer_app_window_add_save_actions(self);

  viewer_app_window_add_save_record_actions(self);

  viewer_app_window_add_gesture_action(self);

  viewer_app_window_add_motion_action(self);

  viewer_app_window_add_scroll_action(self);

  viewer_app_window_add_scale_action(self);

  init_frame_buffer();

  gtk_window_set_icon_name(GTK_WINDOW(self), "viewer");
}

GtkWidget *viewer_app_window_new(ViewerApp *app) {
  return g_object_new(viewer_app_window_get_type(), "application", app, NULL);
}
