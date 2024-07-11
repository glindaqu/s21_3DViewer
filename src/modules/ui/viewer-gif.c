#include "viewer-gif.h"

static void stop_recording(ViewerAppWindow *self) {
  if (self->recording) {
    int result = cgif_close(self->gif);
    if (result != CGIF_OK) {
        g_print("Failed to close GIF file: %d\n", result);
    }
    self->recording = FALSE;
    g_print("Recording finished\n");
  }
}

uint8_t* capture_frame_from_opengl(uint16_t width, uint16_t height) {
    uint8_t* pixels = (uint8_t*)malloc(width * height * 3);
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    return pixels;
}

void add_frame_to_gif(ViewerAppWindow* self, const uint8_t* rgb_data, uint16_t width, uint16_t height) {
    const uint8_t aPalette[] = {self->background_color.red * 255, self->background_color.green * 255, self->background_color.blue * 255,
                    self->edge_color[0] * 255, self->edge_color[1] * 255, self->edge_color[2] * 255,
                    self->point_color[0] * 255, self->point_color[1] * 255, self->point_color[2] * 255
    };
    uint32_t* real_rgb_data = (uint32_t*)calloc(width * height, sizeof(uint32_t));
    for (int i = 0, j = 0; i < width * height * 3; i+=3, j++) {
        real_rgb_data[j] = rgb_data[i] + rgb_data[i + 1] + rgb_data[i + 2]; 
    }   
    g_print("Add frame to GIF\n");
    uint8_t* pImageData = malloc(width * height);
    for (int i = 0; i < width * height; i++) {
        if (abs((int)real_rgb_data[i] - (int)(aPalette[0] + aPalette[1] + aPalette[2])) <= 1) {
            pImageData[i] = 0;   
        }
        else if (abs((int)real_rgb_data[i] - (int)(aPalette[3] + aPalette[4] + aPalette[5])) <= 1) {
            pImageData[i] = 1;
        }
        else if (abs((int)real_rgb_data[i] - (int)(aPalette[6] + aPalette[7] + aPalette[8])) <= 1) {
            pImageData[i] = 2;
        }
    }

    CGIF_FrameConfig frame_config  = {
        .delay = 10,
        .pImageData = pImageData,
    };
    int result = cgif_addframe(self->gif, &frame_config);
    if (result != CGIF_OK) {
        fprintf(stderr, "Failed to add frame to GIF: %d\n", result);
    }
    self->frame_counter++;
    if (self->frame_counter >= 500) {
      stop_recording(self);
    }
    free(real_rgb_data);
    free(pImageData);
}

static void save_gif_callback(GtkFileDialog *dialog, GAsyncResult *result, gpointer user_data) {
    ViewerAppWindow *self = VIEWER_APP_WINDOW(user_data);
    GFile *file = gtk_file_dialog_save_finish(dialog, result, NULL);
    // uint8_t aPalette[] = {0xFF, 0xFF, 0x00,
    //                     0x00, 0xFF, 0x00,
    //                     0x00, 0x00, 0xFF};  
    const uint8_t aPalette[] = {self->background_color.red * 255, self->background_color.green * 255, self->background_color.blue * 255,
                    self->edge_color[0] * 255, self->edge_color[1] * 255, self->edge_color[2] * 255,
                    self->point_color[0] * 255, self->point_color[1] * 255, self->point_color[2] * 255
    };
    if (file) {
        char *filename = g_file_get_path(file);
        g_print("Saving GIF to %s\n", filename);
        CGIF_Config gif_config = {
            .pGlobalPalette = (uint8_t*)aPalette,
            .path = filename,
            .attrFlags = CGIF_ATTR_IS_ANIMATED,
            .width = gtk_widget_get_width(GTK_WIDGET(self->gl_drawing_area)),
            .height = gtk_widget_get_height(GTK_WIDGET(self->gl_drawing_area)),
            .numGlobalPaletteEntries = 3
        };
        self->gif = cgif_newgif(&gif_config);
        if (self->gif) {
            self->recording = TRUE;
            self->frame_counter = 0;
            g_print("Recording started\n");
        } else {
            g_print("Failed to create GIF file\n");
        }
        g_free(filename);
        g_object_unref(file);
    }
}

static void on_save_gif_activate(GSimpleAction *action, GVariant *parameter, gpointer user_data) {
    ViewerAppWindow *self = VIEWER_APP_WINDOW(user_data);
    GtkFileDialog *dialog = gtk_file_dialog_new();
    gtk_file_dialog_set_title(dialog, "Save GIF");
    gtk_file_dialog_set_accept_label(dialog, "Save");

    GFile *initial_folder = g_file_new_for_path(".");
    gtk_file_dialog_set_initial_folder(dialog, initial_folder);
    g_object_unref(initial_folder);

    gtk_file_dialog_save(dialog, GTK_WINDOW(self), NULL, (GAsyncReadyCallback)save_gif_callback, self);
}

void viewer_app_window_add_save_record_actions(ViewerAppWindow *self) {
    g_autoptr(GSimpleAction) save_gif_action = g_simple_action_new("save_gif", NULL);
    g_signal_connect(save_gif_action, "activate", G_CALLBACK(on_save_gif_activate), self);
    g_action_map_add_action(G_ACTION_MAP(self), G_ACTION(save_gif_action));
}
