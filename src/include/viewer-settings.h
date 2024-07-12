#ifndef _VIEWER_SETTINGS_H
#define _VIEWER_SETTINGS_H

#include <gtk/gtk.h>

#include "viewer-app-window.h"


/**
 * @brief Функция открывает окно настроек приложения.
 *
 * @param action Указатель на объект GAction.
 * @param parameter Указатель на объект GVariant, содержащий параметры
 *                  действия (в данном случае NULL).
 * @param self Указатель на объект ViewerAppWindow.
 */
void open_settings_dialog(GAction *action, GVariant *parameter,
                                 ViewerAppWindow *self);
/**
 * @brief Задает перспективное преобразование для окна просмотра.
 *
 * @param self Указатель на объект ViewerAppWindow.
 * @param fovy Угол обзора в градусах по вертикали.
 * @param aspect Соотношение сторон представления.
 * @param nearZ Расстояние до ближней плоскости ограничивающей ортогонали.
 * @param farZ Расстояние до дальней плоскости ограничивающей ортогонали.
 *
 * @details
 * Функция задает перспективное преобразование для окна просмотра OpenGL.
 */
void set_perspective_projection(ViewerAppWindow* self, float fovy, float aspect, float nearZ, float farZ);

/**
 * @brief Задает ортогональное преобразование для окна просмотра.
 *
 * @param self Указатель на объект ViewerAppWindow.
 * @param left Координата левой стороны ортогональной проекции.
 * @param right Координата правой стороны ортогональной проекции.
 * @param bottom Координата нижней стороны ортогональной проекции.
 * @param top Координата верхней стороны ортогональной проекции.
 * @param nearZ Расстояние до ближней плоскости ограничивающей ортогонали.
 * @param farZ Расстояние до дальней плоскости ограничивающей ортогонали.
 *
 * @details
 * Функция задает ортогональное преобразование для окна просмотра OpenGL.
 *
 * @see set_perspective_projection
 */
void set_ortho_projection(ViewerAppWindow* self, float left, float right, float bottom,
                                float top, float nearZ, float farZ);
/**
* @brief Обрабатывает изменение настроек размера точек.
*
* @param self Указатель на объект ViewerAppWindow.
*
* @details
* Функция обрабатывает изменение настроек размера точек.
*/
void apply_point_size_setting(ViewerAppWindow *self);


/**
 * @brief Обрабатывает изменение настроек типа отображения точек.
 *
 * @param self Указатель на объект ViewerAppWindow.
 *
 * @details
 * Функция обрабатывает изменение настроек типа отображения точек.
 */
void apply_point_type_setting(ViewerAppWindow *self);

/**
 * @brief Обрабатывает изменение настроек цвета точек.
 *
 * @param self Указатель на объект ViewerAppWindow.
 *
 * @details
 * Функция обрабатывает изменение настроек цвета точек.
 */
void apply_point_color_setting(ViewerAppWindow *self);

/**
 * @brief Устанавливает толщину границы.
 *
 * @param self Указатель на объект ViewerAppWindow.
 * @param thickness Толщина границы.
 *
 * @details
 * Функция устанавливает толщину границы для окна просмотра.
 */
void set_edge_thickness(ViewerAppWindow* self, float thickness);

/**
 * @brief Устанавливает стиль границы в виде пунктирной линии.
 *
 * @param self Указатель на объект ViewerAppWindow.
 *
 * @details
 * Функция устанавливает стиль границы в виде пунктирной линии для окна
 * просмотра.
 */
void set_edge_strippled(ViewerAppWindow *self);

/**
 * @brief Устанавливает стиль границы в виде сплошной линии.
 *
 * @param self Указатель на объект ViewerAppWindow.
 *
 * @details
 * Функция устанавливает стиль границы в виде сплошной линии для окна
 * просмотра.
 */

/**
 * @brief Устанавливает стиль границы в виде сплошной линии.
 *
 * @param self Указатель на объект ViewerAppWindow.
 *
 * @details
 * Функция устанавливает стиль границы в виде сплошной линии для окна
 * просмотра.
 */
void set_edge_not_strippled(ViewerAppWindow *self);

/**
 * @brief Обрабатывает изменение настроек типа проекции.
 *
 * @param self Указатель на объект ViewerAppWindow.
 *
 * @details
 * Функция обрабатывает изменение настроек типа проекции.
 */
void apply_projection_type_setting(ViewerAppWindow *self);

/**
 * @brief Обрабатывает изменение настроек стиля границы.
 *
 * @param self Указатель на объект ViewerAppWindow.
 *
 * @details
 * Функция обрабатывает изменение настроек стиля границы.
 */
void apply_edge_type_setting(ViewerAppWindow *self);

/**
 * @brief Обрабатывает изменение настроек толщины границы.
 *
 * @param self Указатель на объект ViewerAppWindow.
 *
 * @details
 * Функция обрабатывает изменение настроек толщины границы.
 */
void apply_edge_thickness_setting(ViewerAppWindow *self);

/**
 * @brief Извлекает цвет из варианта и устанавливает его в указанный массив.
 *
 * @param color_variant Вариант, содержащий данные о цвете.
 * @param lineColor Массив, куда нужно установить цвет.
 *
 * @details
 * Функция извлекает данные о цвете из варианта и устанавливает их в указанный
 * массив.
 */
void set_line_color(const GVariant *color_variant, float* lineColor);

/**
 * @brief Обрабатывает изменение настроек цвета границы.
 *
 * @param self Указатель на объект ViewerAppWindow.
 *
 * @details
 * Функция обрабатывает изменение настроек цвета границы.
 */
void apply_edge_color_setting(ViewerAppWindow *self);

/**
 * @brief Обрабатывает изменение настроек цвета фона.
 *
 * @param self Указатель на объект ViewerAppWindow.
 *
 * @details
 * Функция обрабатывает изменение настроек цвета фона.
 */
void apply_background_color_setting(ViewerAppWindow *self);

/**
 * @brief Обрабатывает изменение настроек и вызывает рисование.
 *
 * @param settings Указатель на объект GSettings.
 * @param key Ключ, изменившийся в GSettings.
 * @param self Указатель на объект ViewerAppWindow.
 *
 * @details
 * Функция обрабатывает изменение настроек и вызывает рисование.
 */
void on_settings_changed(GSettings *settings, gchar *key,
                                ViewerAppWindow *self);
#endif  // _VIEWER_SETTINGS_H
