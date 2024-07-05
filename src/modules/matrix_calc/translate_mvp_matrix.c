#include "../../include/matrix_calc.h"

void translate_mvp_matrix(gl_matrix *res, float translate_x, float translate_y,
                          float translate_z) {
  res->mvp[12] = translate_x;
  res->mvp[13] = translate_y;
  res->mvp[14] = translate_z;
}