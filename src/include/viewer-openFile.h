#ifndef _VIEWER_OPENFILE_H
#define _VIEWER_OPENFILE_H

#include "viewer-app-window.h"



/**
 * @brief Callback, вызываемый при открытии файла
 *
 * @param self - указатель на ViewerAppWindow
 * @param file - указатель на GFile, выбранный пользователем
 *
 * @details
 * Callback, который вызывается, когда пользователь выбирает файл
 * в диалоге открытия файла.
 *
 * @note
 * Если файл NULL, то функция выводит сообщение об ошибке
 * и возвращает управление.
 *
 * @author yukikoqo
 */
void open_file(ViewerAppWindow *self, GFile *file);


/**
 * @brief Callback, вызываемый, когда пользователь выбирает файл
 *        в диалоге открытия файла
 *
 * @param source - указатель на GFileDialog
 * @param result - указатель на GAsyncResult
 * @param user_data - указатель на ViewerAppWindow
 *
 * @author yukikoqo
 */
void on_open_response(GObject *source, GAsyncResult *result,
                             gpointer user_data);

/**
 * @brief Callback, вызываемый при активации действия "open" в главном меню
 *
 * @param action - указатель на GAction
 * @param parameter - указатель на GVariant, содержащий параметры
 *                    действия (в данном случае NULL)
 * @param self - указатель на ViewerAppWindow
 *
 * @author yukikoqo
 */
void viewer_app_window__open_file_dialog(GAction *action,
                                                GVariant *parameter,
                                                ViewerAppWindow *self);

#endif  // _VIEWER_OPENFILE_H
