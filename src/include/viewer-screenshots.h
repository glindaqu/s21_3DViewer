#ifndef _VIEWER_SCREENSHOTS_H
#define _VIEWER_SCREENSHOTS_H

#include <gdk-pixbuf/gdk-pixbuf.h>
#include "viewer-app-window.h"

/**
 * @brief Добавление действий сохранения
 *
 * Добавляет действия сохранения BMP и JPEG
 *
 * @param self - указатель на окно
 *
 * @return void
 *
 * @author yukikoqo
 */
void viewer_app_window_add_save_actions(ViewerAppWindow *self);

#endif  // _VIEWER_SCREENSHOTS_H
