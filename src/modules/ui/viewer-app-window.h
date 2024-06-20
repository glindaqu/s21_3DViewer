#ifndef _3DVIEWER_APP_WINDOW_H
#define _3DVIEWER_APP_WINDOW_H

#include <gtk/gtk.h>
#include "viewer-app.h"

G_BEGIN_DECLS

#define VIEWER_APP_WINDOW_TYPE (viewer_app_window_get_type ())
G_DECLARE_FINAL_TYPE(ViewerAppWindow, viewer_app_window, VIEWER, APP_WINDOW, GtkApplicationWindow)

GtkWidget *viewer_app_window_new (ViewerApp *app);

G_END_DECLS

#endif // _3DVIEWER_APP_WINDOW_H
