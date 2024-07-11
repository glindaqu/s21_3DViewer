#include "viewer-screenshots.h"

static void save_image_as_bmp(ViewerAppWindow *self, const char *filename) {
    GtkWidget *gl_area = GTK_WIDGET(self->gl_drawing_area);
    int width = gtk_widget_get_width(gl_area);
    int height = gtk_widget_get_height(gl_area);

    GdkPixbuf *pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, TRUE, 8, width, height);
    if (pixbuf) {
        guchar *pixels = gdk_pixbuf_get_pixels(pixbuf);
        int rowstride = gdk_pixbuf_get_rowstride(pixbuf);

        gtk_gl_area_make_current(GTK_GL_AREA(gl_area));

        glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

        guchar *flipped_pixels = g_new(guchar, rowstride * height);
        for (int y = 0; y < height; y++) {
            memcpy(flipped_pixels + y * rowstride, pixels + (height - y - 1) * rowstride, rowstride);
        }
        memcpy(pixels, flipped_pixels, rowstride * height);
        g_free(flipped_pixels);

        gdk_pixbuf_save(pixbuf, filename, "bmp", NULL, NULL);
        g_object_unref(pixbuf);
    }
}

static void save_bmp_callback(GtkFileDialog *dialog, GAsyncResult *result, gpointer user_data) {
    ViewerAppWindow *self = VIEWER_APP_WINDOW(user_data);
    GFile *file = gtk_file_dialog_save_finish(dialog, result, NULL);
    if (file) {
        char *filename = g_file_get_path(file);
        save_image_as_bmp(self, filename);
        g_free(filename);
        g_object_unref(file);
    }
}

static void on_save_bmp_activate(GSimpleAction *action, GVariant *parameter, gpointer user_data) {
    ViewerAppWindow *self = VIEWER_APP_WINDOW(user_data);
    GtkFileDialog *dialog = gtk_file_dialog_new();
    gtk_file_dialog_set_title(dialog, "Save BMP");
    gtk_file_dialog_set_accept_label(dialog, "Save");

    GFile *initial_folder = g_file_new_for_path(".");
    gtk_file_dialog_set_initial_folder(dialog, initial_folder);
    g_object_unref(initial_folder);

    gtk_file_dialog_save(dialog, GTK_WINDOW(self), NULL, (GAsyncReadyCallback)save_bmp_callback, self);
}

static void save_image_as_jpeg(ViewerAppWindow *self, const char *filename) {
    GtkWidget *gl_area = GTK_WIDGET(self->gl_drawing_area);
    int width = gtk_widget_get_width(gl_area);
    int height = gtk_widget_get_height(gl_area);

    GdkPixbuf *pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, TRUE, 8, width, height);
    if (pixbuf) {
        guchar *pixels = gdk_pixbuf_get_pixels(pixbuf);
        int rowstride = gdk_pixbuf_get_rowstride(pixbuf);

        gtk_gl_area_make_current(GTK_GL_AREA(gl_area));

        glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

        guchar *flipped_pixels = g_new(guchar, rowstride * height);
        for (int y = 0; y < height; y++) {
            memcpy(flipped_pixels + y * rowstride, pixels + (height - y - 1) * rowstride, rowstride);
        }
        memcpy(pixels, flipped_pixels, rowstride * height);
        g_free(flipped_pixels);

        gdk_pixbuf_save(pixbuf, filename, "jpeg", NULL, "quality", "100", NULL);
        g_object_unref(pixbuf);
    }
}

static void save_jpeg_callback(GtkFileDialog *dialog, GAsyncResult *result, gpointer user_data) {
    ViewerAppWindow *self = VIEWER_APP_WINDOW(user_data);
    GFile *file = gtk_file_dialog_save_finish(dialog, result, NULL);
    if (file) {
        char *filename = g_file_get_path(file);
        save_image_as_jpeg(self, filename);
        g_free(filename);
        g_object_unref(file);
    }
}

static void on_save_jpeg_activate(GSimpleAction *action, GVariant *parameter, gpointer user_data) {
    ViewerAppWindow *self = VIEWER_APP_WINDOW(user_data);
    GtkFileDialog *dialog = gtk_file_dialog_new();
    gtk_file_dialog_set_title(dialog, "Save JPEG");
    gtk_file_dialog_set_accept_label(dialog, "Save");

    GFile *initial_folder = g_file_new_for_path(".");
    gtk_file_dialog_set_initial_folder(dialog, initial_folder);
    g_object_unref(initial_folder);

    gtk_file_dialog_save(dialog, GTK_WINDOW(self), NULL, (GAsyncReadyCallback)save_jpeg_callback, self);
}

void viewer_app_window_add_save_actions(ViewerAppWindow *self) {
  g_autoptr(GSimpleAction) save_bmp_action = g_simple_action_new("save_bmp", NULL);
  g_signal_connect(save_bmp_action, "activate", G_CALLBACK(on_save_bmp_activate), self);
  g_action_map_add_action(G_ACTION_MAP(self), G_ACTION(save_bmp_action));

  g_autoptr(GSimpleAction) save_jpeg_action = g_simple_action_new("save_jpeg", NULL);
  g_signal_connect(save_jpeg_action, "activate", G_CALLBACK(on_save_jpeg_activate), self);
  g_action_map_add_action(G_ACTION_MAP(self), G_ACTION(save_jpeg_action));
}
