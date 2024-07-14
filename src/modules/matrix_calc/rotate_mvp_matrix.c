#include <math.h>

#include "../../include/matrix_calc.h"

void rotate_mvp_matrix(gl_matrix *res, float angle1, float angle2,
                       float angle3) {
  float x = angle1 * (M_PI / 180.f);
  float y = angle2 * (M_PI / 180.f);
  float z = angle3 * (M_PI / 180.f);
  float c1 = cosf(x), s1 = sinf(x);
  float c2 = cosf(y), s2 = sinf(y);
  float c3 = cosf(z), s3 = sinf(z);
  float c3c2 = c3 * c2;
  float s3c1 = s3 * c1;
  float c3s2s1 = c3 * s2 * s1;
  float s3s1 = s3 * s1;
  float c3s2c1 = c3 * s2 * c1;
  float s3c2 = s3 * c2;
  float c3c1 = c3 * c1;
  float s3s2s1 = s3 * s2 * s1;
  float c3s1 = c3 * s1;
  float s3s2c1 = s3 * s2 * c1;
  float c2s1 = c2 * s1;
  float c2c1 = c2 * c1;

  /* apply all three Euler angles rotations using the three matrices:
   *
   * ⎡  c3 s3 0 ⎤ ⎡ c2  0 -s2 ⎤ ⎡ 1   0  0 ⎤
   * ⎢ -s3 c3 0 ⎥ ⎢  0  1   0 ⎥ ⎢ 0  c1 s1 ⎥
   * ⎣   0  0 1 ⎦ ⎣ s2  0  c2 ⎦ ⎣ 0 -s1 c1 ⎦
   */
  res->mvp[0] = c3c2;
  res->mvp[4] = s3c1 + c3s2s1;
  res->mvp[8] = s3s1 - c3s2c1;
  res->mvp[12] = 0.f;
  res->mvp[1] = -s3c2;
  res->mvp[5] = c3c1 - s3s2s1;
  res->mvp[9] = c3s1 + s3s2c1;
  res->mvp[13] = 0.f;
  res->mvp[2] = s2;
  res->mvp[6] = -c2s1;
  res->mvp[10] = c2c1;
  res->mvp[14] = 0.f;
  res->mvp[3] = 0.f;
  res->mvp[7] = 0.f;
  res->mvp[11] = 0.f;
  res->mvp[15] = 1.f;
}