#include "../../include/viewer-gif.h"

#include <glib.h>

typedef struct {
  uint8_t* pImageData;
  uint16_t width;
  uint16_t height;
  int delay;
} FrameBufferEntry;

static GQueue* frame_buffer = NULL;
static const int MAX_FRAMES = 5 * 10;

void clear_frame_buffer(void) {
  while (!g_queue_is_empty(frame_buffer)) {
    FrameBufferEntry* entry = g_queue_pop_head(frame_buffer);
    free(entry->pImageData);
    free(entry);
  }
}

void init_frame_buffer(void) {
  if (frame_buffer != NULL) {
    clear_frame_buffer();
  } else {
    frame_buffer = g_queue_new();
  }
}

void free_frame_buffer(void) {
  clear_frame_buffer();
  g_queue_free(frame_buffer);
  frame_buffer = NULL;
}

void write_frames_to_gif(ViewerAppWindow* self);
static void stop_recording(ViewerAppWindow* self) {
  if (self->recording) {
    int result = cgif_close(self->gif);
    if (result != CGIF_OK) {
      g_print("Failed to close GIF file: %d\n", result);
    }
    if (self->gif == NULL) {
      g_print("Failed to recreate GIF file\n");
    }
    self->recording = FALSE;
    g_print("Recording finished\n");
  }
}
uint8_t* capture_frame_from_opengl(uint16_t width, uint16_t height) {
  uint8_t* pixels = (uint8_t*)malloc(width * height * 3);
  if (!pixels) {
    g_print("Failed to allocate memory for frame capture\n");
    return NULL;
  }

  glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);

  uint8_t* flippedPixels = (uint8_t*)malloc(width * height * 3);
  if (!flippedPixels) {
    free(pixels);
    g_print("Failed to allocate memory for frame flip capture\n");
    return NULL;
  }

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      int srcIndex = (y * width + x) * 3;
      int destIndex = ((height - 1 - y) * width + x) * 3;
      flippedPixels[destIndex] = pixels[srcIndex];
      flippedPixels[destIndex + 1] = pixels[srcIndex + 1];
      flippedPixels[destIndex + 2] = pixels[srcIndex + 2];
    }
  }

  free(pixels);

  return flippedPixels;
}

void add_frame_to_buffer(ViewerAppWindow* self, const uint8_t* rgb_data,
                         uint16_t width, uint16_t height) {
  g_print("Add frame to buffer\n");
  uint8_t* pImageData = malloc(width * height);
  const int32_t palette_red = self->background_color.red * 255 +
                              self->background_color.green * 255 +
                              self->background_color.blue * 255;
  const int32_t palette_edge = self->edge_color[0] * 255 +
                               self->edge_color[1] * 255 +
                               self->edge_color[2] * 255;
  const int32_t palette_point = self->point_color[0] * 255 +
                                self->point_color[1] * 255 +
                                self->point_color[2] * 255;
  const int32_t abs_threshold = 1;
  for (int i = 0, j = 0; i < width * height * 3; i += 3, j++) {
    int32_t rgb_sum = rgb_data[i] + rgb_data[i + 1] + rgb_data[i + 2];
    if (abs(rgb_sum - palette_red) <= abs_threshold) {
      pImageData[j] = 0;
    } else if (abs(rgb_sum - palette_edge) <= abs_threshold) {
      pImageData[j] = 1;
    } else if (abs(rgb_sum - palette_point) <= abs_threshold) {
      pImageData[j] = 2;
    }
  }

  FrameBufferEntry* entry = malloc(sizeof(FrameBufferEntry));
  if (entry == NULL) {
    g_print("Failed to allocate memory for FrameBufferEntry\n");
    free(pImageData);
    return;
  }
  entry->pImageData = pImageData;
  entry->width = width;
  entry->height = height;
  entry->delay = 10;

  g_queue_push_tail(frame_buffer, entry);

  if (g_queue_get_length(frame_buffer) >= MAX_FRAMES) {
    write_frames_to_gif(self);
  }
}

void write_frames_to_gif(ViewerAppWindow* self) {
  while (!g_queue_is_empty(frame_buffer)) {
    FrameBufferEntry* entry = g_queue_pop_head(frame_buffer);
    CGIF_FrameConfig frame_config = {
        .delay = entry->delay,
        .pImageData = entry->pImageData,
    };
    cgif_addframe(self->gif, &frame_config);
    free(entry->pImageData);
    free(entry);
  }
  stop_recording(self);
}

static void save_gif_callback(GtkFileDialog* dialog, GAsyncResult* result,
                              gpointer user_data) {
  ViewerAppWindow* self = VIEWER_APP_WINDOW(user_data);
  GFile* file = gtk_file_dialog_save_finish(dialog, result, NULL);
  const uint8_t aPalette[] = {
      self->background_color.red * 255,  self->background_color.green * 255,
      self->background_color.blue * 255, self->edge_color[0] * 255,
      self->edge_color[1] * 255,         self->edge_color[2] * 255,
      self->point_color[0] * 255,        self->point_color[1] * 255,
      self->point_color[2] * 255};
  if (file) {
    char* filename = g_file_get_path(file);
    g_print("Saving GIF to %s\n", filename);
    CGIF_Config gif_config = {
        .pGlobalPalette = (uint8_t*)aPalette,
        .path = filename,
        .attrFlags = CGIF_ATTR_IS_ANIMATED,
        .width = gtk_widget_get_width(GTK_WIDGET(self->gl_drawing_area)),
        .height = gtk_widget_get_height(GTK_WIDGET(self->gl_drawing_area)),
        .numGlobalPaletteEntries = 3};
    self->gif = cgif_newgif(&gif_config);
    if (self->gif) {
      self->recording = TRUE;
      self->frame_counter = 0;
      g_print("Recording started\n");
      init_frame_buffer();
    } else {
      g_print("Failed to create GIF file\n");
    }
    g_free(filename);
    g_object_unref(file);
  }
}

static void on_save_gif_activate(UNUSED GSimpleAction* action,
                                 UNUSED GVariant* parameter,
                                 gpointer user_data) {
  ViewerAppWindow* self = VIEWER_APP_WINDOW(user_data);
  GtkFileDialog* dialog = gtk_file_dialog_new();
  gtk_file_dialog_set_title(dialog, "Save GIF");
  gtk_file_dialog_set_accept_label(dialog, "Save");

  GFile* initial_folder = g_file_new_for_path(".");
  gtk_file_dialog_set_initial_folder(dialog, initial_folder);
  g_object_unref(initial_folder);

  gtk_file_dialog_save(dialog, GTK_WINDOW(self), NULL,
                       (GAsyncReadyCallback)save_gif_callback, self);
}

void viewer_app_window_add_save_record_actions(ViewerAppWindow* self) {
  g_autoptr(GSimpleAction) save_gif_action =
      g_simple_action_new("save_gif", NULL);
  g_signal_connect(save_gif_action, "activate",
                   G_CALLBACK(on_save_gif_activate), self);
  g_action_map_add_action(G_ACTION_MAP(self), G_ACTION(save_gif_action));
}
