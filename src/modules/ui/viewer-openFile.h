#ifndef _VIEWER_OPENFILE_H
#define _VIEWER_OPENFILE_H

#include "viewer-app-window.h"

void open_file(ViewerAppWindow *self, GFile *file);

void on_open_response(GObject *source, GAsyncResult *result,
                             gpointer user_data);

void viewer_app_window__open_file_dialog(GAction *action,
                                                GVariant *parameter,
                                                ViewerAppWindow *self);

#endif  // _VIEWER_OPENFILE_H
