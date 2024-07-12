#include "../../include/viewer-modelMovement.h"

void gl_button_press_event(GtkGestureClick *gesture, int n_press, double x,
                           double y, ViewerAppWindow *self) {
  if (gtk_gesture_single_get_current_button(GTK_GESTURE_SINGLE(gesture)) ==
      GDK_BUTTON_PRIMARY) {
    self->mouse_dragging = TRUE;
    self->last_mouse_x = x;
    self->last_mouse_y = y;
  }
}

void gl_button_release_event(GtkGestureClick *gesture, int n_press, double x,
                             double y, ViewerAppWindow *self) {
  if (gtk_gesture_single_get_current_button(GTK_GESTURE_SINGLE(gesture)) ==
      GDK_BUTTON_PRIMARY) {
    self->mouse_dragging = FALSE;
  }
}

void gl_motion_notify_event(GtkEventControllerMotion *controller, double x,
                            double y, ViewerAppWindow *self) {
  if (self->mouse_dragging) {
    double dx = x - self->last_mouse_x;
    double dy = y - self->last_mouse_y;

    self->matrix_movement.rotation_angles[X_AXIS] += dy * 0.5;
    self->matrix_movement.rotation_angles[Y_AXIS] += dx * 0.5;

    move_mvp_matrix(self->mvp_matrix, &self->matrix_movement);

    self->last_mouse_x = x;
    self->last_mouse_y = y;

    gtk_widget_queue_draw(self->gl_drawing_area);
  }
}

void gl_scroll_event(GtkEventControllerScroll *controller, double dx, double dy,
                     ViewerAppWindow *self) {
  GdkModifierType state = gtk_event_controller_get_current_event_state(
      GTK_EVENT_CONTROLLER(controller));
  if (state & GDK_SHIFT_MASK) {
    self->matrix_movement.scale_vector[X_AXIS] += dy;
    self->matrix_movement.scale_vector[Y_AXIS] += dy;
    self->matrix_movement.scale_vector[Z_AXIS] += dy;

    for (int i = 0; i < N_AXES; i++) {
      self->matrix_movement.scale_vector[i] =
          fmax(fmin(self->matrix_movement.scale_vector[i] + dy, 1.0), 0.1);
    }

    move_mvp_matrix(self->mvp_matrix, &self->matrix_movement);

    gtk_widget_queue_draw(self->gl_drawing_area);

  } else if (state & GDK_ALT_MASK) {
    self->matrix_movement.rotation_angles[Z_AXIS] += (dx + dy) * 5.0;

    move_mvp_matrix(self->mvp_matrix, &self->matrix_movement);

    gtk_widget_queue_draw(self->gl_drawing_area);
  }
}

void on_scale_value_changed(GtkRange *range, ViewerAppWindow *self) {
  self->matrix_movement.translation_vector[X_AXIS] =
      gtk_range_get_value(GTK_RANGE(self->x_scale));
  self->matrix_movement.translation_vector[Y_AXIS] =
      gtk_range_get_value(GTK_RANGE(self->y_scale));
  self->matrix_movement.translation_vector[Z_AXIS] =
      gtk_range_get_value(GTK_RANGE(self->z_scale));

  move_mvp_matrix(self->mvp_matrix, &self->matrix_movement);

  gtk_widget_queue_draw(self->gl_drawing_area);
}
