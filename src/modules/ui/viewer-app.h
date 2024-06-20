#ifndef _3DVIEWER_APP_H
#define _3DVIEWER_APP_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define VIEWER_APP_TYPE (viewer_app_get_type ())
G_DECLARE_FINAL_TYPE(ViewerApp, viewer_app, VIEWER, APP, GtkApplication)
GtkApplication *viewer_app_new (void);

G_END_DECLS


#endif // _3DVIEWER_APP_H
