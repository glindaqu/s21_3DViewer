#include "viewer-setLabel.h"

const char *format_number(int number) {
  static char buffer[20];
  if (number >= 1000000) {
    snprintf(buffer, sizeof(buffer), "%.1fM", number / 1000000.0);
  } else if (number >= 1000) {
    snprintf(buffer, sizeof(buffer), "%.1fK", number / 1000.0);
  } else {
    snprintf(buffer, sizeof(buffer), "%d", number);
  }
  return buffer;
}

void update_label_text(AppData *self) {
  const char *basename = g_path_get_basename(self->obj_file->fileName);
  const char *formatted_vertices = format_number(self->obj_file->verticesCount);
  const char *formatted_surfaces = format_number(self->obj_file->surfacesCount);
  char *model_name;
  if (strlen(basename) > 15) {
    model_name = g_strdup_printf("*...[.obj]");
  } else {
    model_name = g_strdup_printf("%s", basename);
  }

  char *label_text =
      g_strdup_printf("Model: %s, Vertices: %3s, Surfaces: %3s", model_name,
                      formatted_vertices, formatted_surfaces);
  gtk_label_set_text(GTK_LABEL(self->model_name_label), label_text);
  g_free(label_text);
  g_free(model_name);
}
