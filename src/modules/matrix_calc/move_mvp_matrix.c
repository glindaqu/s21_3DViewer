#include <stdio.h>

#include "../../include/matrix_calc.h"

void move_mvp_matrix(gl_matrix *res, mvp_matrix_movement_t *movement) {
  gl_matrix rotation_matrix;
  gl_matrix temp_matrix;
  gl_matrix translate_matrix;
  init_mvp_matrix(&rotation_matrix);
  init_mvp_matrix(&temp_matrix);
  init_mvp_matrix(&translate_matrix);

  translate_mvp_matrix(&temp_matrix, movement->translation_vector[0],
                       movement->translation_vector[1],
                       movement->translation_vector[2]);
  rotate_mvp_matrix(&rotation_matrix, movement->rotation_angles[X_AXIS],
                    movement->rotation_angles[Y_AXIS],
                    movement->rotation_angles[Z_AXIS]);
  multiply_matrices(&translate_matrix, &rotation_matrix, res);
  for (int i = 0; i < 16; ++i) {
    printf("%f ", res->mvp[i]);
    if ((i + 1) % 4 == 0) {
      printf("\n");
    }
  }
  // multiply_matrices(&rotation_matrix, &temp_matrix, res);
}