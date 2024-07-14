#ifndef _VIEWER_APP_SETTINGS_H
#define _VIEWER_APP_SETTINGS_H
#include <gtk/gtk.h>

#include "viewer-app-window.h"

G_BEGIN_DECLS
/** @brief Тип настроек */
#define VIEWER_APP_SETTINGS_TYPE (viewer_app_settings_get_type())
/** @brief Наследование класса настроек */
G_DECLARE_FINAL_TYPE(ViewerAppSettings, viewer_app_settings, VIEWER,
                     APP_SETTINGS, GtkDialog)
/** @brief Функция создания настроек */
ViewerAppSettings *viewer_app_settings_new(ViewerAppWindow *win);

/**
 * @brief Cтруктура настроек
 * @param parent - структура настроек
 * @param settings - указатель на настройки
 * @param projection - виджет проекции
 * @param edge_type - виджет типа линий
 * @param edge_color - виджет цвета линий
 * @param edge_thickness_spin - виджет толщины линий
 * @param point_type - виджет типа точек
 * @param point_color - виджет цвета точек
 * @param point_size - виджет размера точек
 * @param background_color - виджет цвета фона
 * @author yukikoqo
 */
struct _ViewerAppSettings {
  GtkDialog parent;

  GSettings *settings;

  GtkWidget *projection;
  GtkWidget *edge_type;
  GtkWidget *edge_color;
  GtkWidget *edge_thickness_spin;
  GtkWidget *point_type;
  GtkWidget *point_color;
  GtkWidget *point_size;
  GtkWidget *background_color;
};

G_END_DECLS

#endif  // _VIEWER_APP_SETTINGS_H
