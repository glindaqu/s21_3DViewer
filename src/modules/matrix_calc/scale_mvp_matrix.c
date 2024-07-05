#include <stdio.h>

#include "../../include/matrix_calc.h"

void scale_mvp_matrix(gl_matrix *res, float x, float y, float z) {
  printf("Scale: %f %f %f\n", x, y, z);
  res->mvp[0] = x;
  res->mvp[5] = y;
  res->mvp[10] = z;
}