#ifndef _3DVIEWER_APP_WINDOW_H
#define _3DVIEWER_APP_WINDOW_H

#include <cgif.h>
#include <cglm/cglm.h>
#include <gtk/gtk.h>

#include "matrix_calc.h"
#include "parser.h"
#include "viewer-app.h"
#include "viewer-glfuncs.h"

G_BEGIN_DECLS

/** @brief Тип окна */
#define VIEWER_APP_WINDOW_TYPE (viewer_app_window_get_type())
/** @brief Наследование класса окна */
G_DECLARE_FINAL_TYPE(ViewerAppWindow, viewer_app_window, VIEWER, APP_WINDOW,
                     GtkApplicationWindow)

/** @brief Функция создания окна */
GtkWidget *viewer_app_window_new(ViewerApp *app);

/**
 * @brief Структура окна
 * @param GtkApplicationWindow - структура окна
 * @param header_bar - хедер бар
 * @param settings - настройки
 * @param matrix_movement - структура перемещения
 * @param primary_menu - основное меню
 * @param open_button - кнопка открытия
 * @param obj_file - указатель на структуру ObjFile
 * @param projection_matrix - матрица проекции
 * @param mvp_matrix - матрица преобразования
 * @param gl_buffers - структура буферов
 * @param shader_vars - структура переменных шейдера
 * @param recording - флаг записи в GIF
 * @param frame_counter - счетчик кадров
 * @param gif - указатель на GIF
 * @param gl_drawing_area - OpenGL панель для рендеринга
 * @param model_name_label - название модели
 * @param x_scale - масштаб по x
 * @param y_scale - масштаб по y
 * @param z_scale - масштаб по z
 * @param point_type - тип точек
 * @param point_size - размер точек
 * @param edge_thickness - толщина линии
 * @param edge_color - цвет линии
 * @param point_color - цвет точек
 * @param background_color - цвет фона
 * @param pattern - паттерн линий
 * @param factor - коэффициент сглаживания
 * @param mouse_dragging - флаг перетаскивания мыши
 * @param last_mouse_x - последнее положение мыши по x
 * @param last_mouse_y - последнее положение мыши по y
 * @see ViewerAppWindow
 * @author yukikoqo
 */
struct _ViewerAppWindow {
  GtkApplicationWindow parent_instance;

  GtkHeaderBar *header_bar;
  GSettings *settings;

  mvp_matrix_movement_t matrix_movement;

  GtkWidget *primary_menu;
  GtkButton *open_button;

  ObjFile_t *obj_file;

  mat4 projection_matrix;
  gl_matrix *mvp_matrix;

  // Gif record
  gboolean recording;
  guint frame_counter;
  CGIF *gif;

  GtkWidget *gl_drawing_area;

  GtkLabel *model_name_label;

  GtkScale *x_scale;
  GtkScale *y_scale;
  GtkScale *z_scale;

  /* GL objects */

  GLbuffers_t gl_buffers;
  GLshader_vars_t shader_vars;

  /* Settings */
  gint point_type;
  gfloat point_size;
  gfloat edge_thickness;
  gfloat edge_color[3];
  gfloat point_color[3];
  GdkRGBA background_color;

  GLushort pattern;
  GLfloat factor;

  /* Mouse state */
  gboolean mouse_dragging;
  double last_mouse_x;
  double last_mouse_y;
};

G_END_DECLS

/**
 * @brief Структура окна
 * @param parent_class - структура класса окна
 * @see ViewerAppWindow
 */
struct _ViewerAppWindowClass {
  GtkApplicationWindowClass parent_class;
};

#endif  // _3DVIEWER_APP_WINDOW_H
