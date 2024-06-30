#include "../../include/matrix_calc.h"

void init_mvp_matrix(gl_matrix *matrix) {
  matrix->mvp[0] = 1.f;
  matrix->mvp[4] = 0.f;
  matrix->mvp[8] = 0.f;
  matrix->mvp[12] = 0.f;
  matrix->mvp[1] = 0.f;
  matrix->mvp[5] = 1.f;
  matrix->mvp[9] = 0.f;
  matrix->mvp[13] = 0.f;
  matrix->mvp[2] = 0.f;
  matrix->mvp[6] = 0.f;
  matrix->mvp[10] = 1.f;
  matrix->mvp[14] = 0.f;
  matrix->mvp[3] = 0.f;
  matrix->mvp[7] = 0.f;
  matrix->mvp[11] = 0.f;
  matrix->mvp[15] = 1.f;
}