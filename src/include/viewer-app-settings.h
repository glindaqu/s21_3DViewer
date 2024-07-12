#ifndef _VIEWER_APP_SETTINGS_H
#define _VIEWER_APP_SETTINGS_H
#include <gtk/gtk.h>

#include "viewer-app-window.h"

G_BEGIN_DECLS

#define VIEWER_APP_SETTINGS_TYPE (viewer_app_settings_get_type())
G_DECLARE_FINAL_TYPE(ViewerAppSettings, viewer_app_settings, VIEWER,
                     APP_SETTINGS, GtkDialog)

ViewerAppSettings *viewer_app_settings_new(ViewerAppWindow *win);

G_END_DECLS

#endif  // _VIEWER_APP_SETTINGS_H
