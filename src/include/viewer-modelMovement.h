#ifndef _VIEWER_MODELMOVEMENT_H
#define _VIEWER_MODELMOVEMENT_H

#include <gtk/gtk.h>
#include "viewer-app-window.h"

/** 
 * @brief Обработчики нажатия кнопки мыши
 * 
 * @param gesture - указатель на обработчик
 * @param n_press - количество нажатий
 * @param x - координата x
 * @param y - координата y
 * @param self - указатель на окно
 * @return void
 *
 * @author yukikoqo
*/
void gl_button_press_event(GtkGestureClick *gesture, int n_press,
                                  double x, double y, ViewerAppWindow *self);

/**
 * @brief Обработчик отпускания кнопки мыши
 * 
 * @param gesture - указатель на обработчик
 * @param n_press - количество нажатий
 * @param x - координата x
 * @param y - координата y
 * @param self - указатель на окно
 * @return void
 *
 * @author yukikoqo
 */
void gl_button_release_event(GtkGestureClick *gesture, int n_press,
                                    double x, double y, ViewerAppWindow *self);

/**
 * @brief Обработчик движения мыши
 * 
 * @param controller - указатель на обработчик
 * @param x - координата x
 * @param y - координата y
 * @param self - указатель на окно
 * @return void
 *
 * @author yukikoqo
 */
void gl_motion_notify_event(GtkEventControllerMotion *controller,
                                   double x, double y, ViewerAppWindow *self);

/**
 * @brief Обработчик события прокрутки
 *
 * @param controller указатель на обработчик события прокрутки
 * @param dx изменение положения по оси X
 * @param dy изменение положения по оси Y
 * @param self указатель на окно приложения
 * @return void
 *
 * @author yukikoqo
 */
void gl_scroll_event(GtkEventControllerScroll *controller, double dx,
                            double dy, ViewerAppWindow *self);
                            
                           
/**
 * @brief Обработчик события изменения значения ползунка
 * 
 * @param range - указатель на ползунок
 * @param self - указатель на окно
 * @return void
 *
 * @author yukikoqo
 */
void on_scale_value_changed(GtkRange *range, ViewerAppWindow *self);

#endif  // _VIEWER_MODELMOVEMENT_H
