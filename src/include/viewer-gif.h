#ifndef _VIEWER_GIF_H
#define _VIEWER_GIF_H

#include <cgif.h>

#include "viewer-app-window.h"

#define FPS 10
#define DURATION 5
#define FRAME_COUNT (FPS * DURATION)

/**
 * @brief Добавление кадра в буфер
 *
 * @param self - указатель на окно
 * @param rgb_data - данные кадра в формате RGB (uint8_t*)
 * @param width - ширина кадра
 * @param height - высота кадра
 * @return void
 *
 * @author yukikoqo, glindaqu
 */
void add_frame_to_buffer(ViewerAppWindow* self, const uint8_t* rgb_data,
                         uint16_t width, uint16_t height);

/**
 * @brief Забор кадра из OpenGL
 *
 * @param width - ширина кадра
 * @param height - высота кадра
 * @return данные кадра в формате RGB (uint8_t*)
 *
 * @author yukikoqo, glindaqu
 */
uint8_t* capture_frame_from_opengl(uint16_t width, uint16_t height);

/**
 * @brief Запись GIF
 *
 * @param self - указатель на окно
 * @return void
 *
 * @author yukikoqo
 */
void viewer_app_window_add_save_record_actions(ViewerAppWindow* self);

/**
 * @brief Инициализация буфера кадров
 *
 * @return void
 *
 * @author yukikoqo
 */
void init_frame_buffer(void);

/**
 * @brief Очистка буфера кадров
 *
 * @return void
 *
 * @author yukikoqo
 */
void free_frame_buffer(void);
#endif  // _VIEWER_GIF_H
