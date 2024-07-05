#ifndef _MATRIX_CALC_H
#define _MATRIX_CALC_H

#define _USE_MATH_DEFINES
#include <math.h>
#include <string.h>

enum { X_AXIS, Y_AXIS, Z_AXIS, N_AXES };

/** @brief Тип векторов для поворота, перемещения и масштабирования
 * @param rotation_angles - массив углов поворота
 * @param translation_vector - вектор перемещения
 * @param scale_vector - вектор масштабирования
 *
 * @author yukikoqo
 */
typedef struct {
  double rotation_angles[N_AXES];
  double translation_vector[3];
  double scale_vector[3];
} mvp_matrix_movement_t;

/** @brief Матрица преобразования
 * @param mvp - Матрица
 *
 * @author yukikoqo
 */
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
/**
 * @brief Матрица перемещения
 *
 * @param mat - указатель на матрицу
 * @param translate_x - смещение по x
 * @param translate_y - смещение по y
 * @param translate_z - смещение по z
 * @return void
 *
 * @author yukikoqo
 */

void move_mvp_matrix(gl_matrix *res, mvp_matrix_movement_t *movement);

void multiply_matrices(const gl_matrix *a, const gl_matrix *b,
                       gl_matrix *result);

void translate_mvp_matrix(gl_matrix *res, float translate_x, float translate_y,
                          float translate_z);

#endif  // _MATRIX_CALC_H
