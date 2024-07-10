#ifndef _VIEWER_MODELMOVEMENT_H
#define _VIEWER_MODELMOVEMENT_H

#include <gtk/gtk.h>
#include "viewer-app-window.h"

void gl_button_press_event(GtkGestureClick *gesture, int n_press,
                                  double x, double y, ViewerAppWindow *self);

void gl_button_release_event(GtkGestureClick *gesture, int n_press,
                                    double x, double y, ViewerAppWindow *self);

void gl_motion_notify_event(GtkEventControllerMotion *controller,
                                   double x, double y, ViewerAppWindow *self);

void gl_scroll_event(GtkEventControllerScroll *controller, double dx,
                            double dy, ViewerAppWindow *self);
                            
void on_scale_value_changed(GtkRange *range, ViewerAppWindow *self);

#endif  // _VIEWER_MODELMOVEMENT_H
