#ifndef _VIEWER_GLAREA_H_
#define _VIEWER_GLAREA_H_

#include <epoxy/gl.h>
#include <gtk/gtk.h>

#include "viewer-app-window.h"

void gl_init(ViewerAppWindow *self);
void gl_fini(ViewerAppWindow *self);
void gl_model_draw(ViewerAppWindow *self);
gboolean gl_draw(ViewerAppWindow *self);

#endif  // _VIEWER_GLAREA_H_
