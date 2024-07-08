#include "../../include/matrix_calc.h"

void multiply_matrices(const gl_matrix *a, const gl_matrix *b,
                       gl_matrix *result) {
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      result->mvp[i * 4 + j] = 0.0f;
      for (int k = 0; k < 4; ++k) {
        result->mvp[i * 4 + j] += a->mvp[i * 4 + k] * b->mvp[k * 4 + j];
      }
    }
  }
}