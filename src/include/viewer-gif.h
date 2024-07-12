#ifndef _VIEWER_GIF_H
#define _VIEWER_GIF_H

#include "viewer-app-window.h"
#include <cgif.h>

#define FPS 10
#define DURATION 5
#define FRAME_COUNT (FPS * DURATION)

void add_frame_to_buffer(ViewerAppWindow* self, const uint8_t* rgb_data, uint16_t width, uint16_t height);

uint8_t* capture_frame_from_opengl(uint16_t width, uint16_t height);

void viewer_app_window_add_save_record_actions(ViewerAppWindow *self);

void init_frame_buffer(void);

void free_frame_buffer(void);
#endif  // _VIEWER_GIF_H
