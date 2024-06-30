/*
 * DO NOT EDIT THIS FILE. Generated by checkmk.
 * Edit the original source file "tests/parser/parser.check" instead.
 */

#include <check.h>

#line 1 "tests/parser/parser.check"
#include <stdio.h>
#include <stdlib.h>

#include "../../include/parser.h"

START_TEST(parser_test_1) {
#line 6
  ObjFile_t* objFile = calloc(1, sizeof(ObjFile_t));
  objFile->fileName = "./tests/parser/cube.obj";
  ParserReturnCode result = parse(objFile);
  removeObjFile(objFile);
  free(objFile);
  fail_unless(result == OK);
}
END_TEST

START_TEST(parser_test_2) {
#line 14
  const char path[] = "./tests/parser/test2.obj";
  ObjFile_t objFile;
  objFile.fileName = path;
  ParserReturnCode result = parse(&objFile);
  fail_unless(result == FILE_DOES_NOT_EXISTS);
}
END_TEST

START_TEST(parser_test_3) {
#line 21
  ObjFile_t* objFile = calloc(1, sizeof(ObjFile_t));
  objFile->fileName =
      "/home/megadrage/Development/21School/C8_3DViewer_v1.0-1/src/tests/"
      "parser/cube.obj";
  ParserReturnCode result = parse(objFile);
  Vertex_t vertex[8] = {0};
  /*#8 vertices, 6 faces
  v -1.00000000 -1.00000000 -1.00000000
  v -1.00000000 -1.00000000 1.00000000
  v -1.00000000 1.00000000 -1.00000000
  v -1.00000000 1.00000000 1.00000000
  v 1.00000000 -1.00000000 -1.00000000
  v 1.00000000 -1.00000000 1.00000000
  v 1.00000000 1.00000000 -1.00000000
  v 1.00000000 1.00000000 1.00000000
  */
  vertex[0].x = -0.5;
  vertex[0].y = -0.5;
  vertex[0].z = -0.5;
  vertex[1].x = -0.5;
  vertex[1].y = -0.5;
  vertex[1].z = 0.5;
  vertex[2].x = -0.5;
  vertex[2].y = 0.5;
  vertex[2].z = -0.5;
  vertex[3].x = -0.5;
  vertex[3].y = 0.5;
  vertex[3].z = 0.5;
  vertex[4].x = 0.5;
  vertex[4].y = -0.5;
  vertex[4].z = -0.5;
  vertex[5].x = 0.5;
  vertex[5].y = -0.5;
  vertex[5].z = 0.5;
  vertex[6].x = 0.5;
  vertex[6].y = 0.5;
  vertex[6].z = -0.5;
  vertex[7].x = 0.5;
  vertex[7].y = 0.5;
  vertex[7].z = 0.5;
  for (int i = 0; i < 8; i++) {
    printf("vertex %d: %f %f %f\n", i, objFile->vertices[i]->x,
           objFile->vertices[i]->y, objFile->vertices[i]->z);
    ck_assert_int_eq(objFile->vertices[i]->x, vertex[i].x);
  }
  removeObjFile(objFile);
  free(objFile);
  fail_unless(result == OK);
}
END_TEST

START_TEST(parser_test_4) {
#line 52
  ObjFile_t* objFile = calloc(1, sizeof(ObjFile_t));
  objFile->fileName = "./tests/parser/cube2.obj";
  ParserReturnCode result = parse(objFile);
  for (int i = 0; i < objFile->surfacesCount; i++) {
    printf("surfaces %d: %d %d %d\n", i,
           objFile->surfaces[i]->verticesIndices[0],
           objFile->surfaces[i]->verticesIndices[1],
           objFile->surfaces[i]->verticesIndices[2]);
  }
  removeObjFile(objFile);
  free(objFile);
  fail_unless(result == OK);
}
END_TEST

int main(void) {
  Suite* s1 = suite_create("Core");
  TCase* tc1_1 = tcase_create("Core");
  SRunner* sr = srunner_create(s1);
  int nf;

  suite_add_tcase(s1, tc1_1);
  tcase_add_test(tc1_1, parser_test_1);
  tcase_add_test(tc1_1, parser_test_2);
  tcase_add_test(tc1_1, parser_test_3);
  tcase_add_test(tc1_1, parser_test_4);

  srunner_run_all(sr, CK_ENV);
  nf = srunner_ntests_failed(sr);
  srunner_free(sr);

  return nf == 0 ? 0 : 1;
}