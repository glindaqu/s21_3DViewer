#ifndef _MATRIX_CALC_H
#define _MATRIX_CALC_H

#define _USE_MATH_DEFINES
#include <math.h>

typedef struct {
  float mvp[16];
} gl_matrix;

/**
 * @brief Инициализация матрицы mvp
 *
 * @param mvp - указатель на матрицу
 * @return void
 *
 * @author yukikoqo
 */
void init_mvp_matrix(gl_matrix *mvp);

/**
 * @brief Матрица поворота
 *
 * @param res - указатель на матрицу
 * @param angle1 - угол поворота x
 * @param angle2 - угол поворота y
 * @param angle3 - угол поворота z
 * @return void
 *
 * @author yukikoqo
 */
void rotate_mvp_matrix(gl_matrix *res, float angle1, float angle2,
                       float angle3);

#endif  // _MATRIX_CALC_H