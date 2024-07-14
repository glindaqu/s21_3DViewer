#ifndef _3DVIEWER_APP_H
#define _3DVIEWER_APP_H

#include <gtk/gtk.h>
#define UNUSED __attribute__((unused))

G_BEGIN_DECLS
/** @brief Тип приложения */
#define VIEWER_APP_TYPE (viewer_app_get_type())
/** @brief Наследование класса приложения */
G_DECLARE_FINAL_TYPE(ViewerApp, viewer_app, VIEWER, APP, GtkApplication)
/** @brief Функция создания приложения */
GtkApplication *viewer_app_new(void);

/**
 * @brief Класс приложения
 * @param parent_instance - родительский класс
 * @param window - окно
 * @author yukikoqo
 */
struct _ViewerApp {
  GtkApplication parent_instance;
  GtkWidget *window;
};

/**
 * @brief Класс приложения
 * @param parent_class - родительский класс
 */
struct _ViewerAppClass {
  GtkApplicationClass parent_class;
};

G_END_DECLS

#endif  // _3DVIEWER_APP_H
