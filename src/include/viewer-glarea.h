#ifndef _VIEWER_GLAREA_H_
#define _VIEWER_GLAREA_H_

#include <epoxy/gl.h>
#include <gtk/gtk.h>

#include "viewer-app-window.h"

/** 
 * @brief Инициализация OpenGL передается в качестве callback функции realize
 * 
 * @param self - указатель на окно
 * @return void
 *
 * @author yukikoqo
*/
void gl_init(ViewerAppWindow *self);
/** 
 * @brief Деинициализация OpenGL передается в качестве callback функции unrealize
 *
 * @param self - указатель на окно
 * @return void
 *
 * @author yukikoqo
*/
void gl_fini(ViewerAppWindow *self);
/** 
 * @brief Отрисовка модели OpenGL
 *
 * @param self - указатель на окно
 * @return void
 *
 * @author yukikoqo
*/
void gl_model_draw(ViewerAppWindow *self);
/** 
 * @brief Отрисовка OpenGL передается в качестве callback функции render
 *
 * @param self - указатель на окно
 * @return void
 *
 * @author yukikoqo
*/
gboolean gl_draw(ViewerAppWindow *self);

#endif  // _VIEWER_GLAREA_H_
