#include "test.h"

START_TEST(test_check_facets_invalid_input) {
  system("echo \"f 1/2/3 4/5/6 7/8/9/9\" > test.obj");
  int status = processObjFile("test.obj", &vertexIndices, &vertexIndicesCount,
                              &vertices, &vertexCount);
  ck_assert_int_eq(status, ERROR);
  system("rm test.obj");
  clean();
}
END_TEST

START_TEST(test_check_vertex_valid) {
  system("echo \"v -1.0 2.0 -3.0\n\" > test.obj");
  int status = processObjFile("test.obj", &vertexIndices, &vertexIndicesCount,
                              &vertices, &vertexCount);
  ck_assert_int_eq(status, OK);
  system("rm test.obj");
  clean();
}
END_TEST

START_TEST(test_check_vertex) {
  float result[24] = {1.0, -1.0, -1.0, 1.0,  -1.0, 1.0,  -1.0, -1.0,
                      1.0, -1.0, -1.0, -1.0, 1.0,  1.0,  -1.0, 1.0,
                      1.0, 1.0,  -1.0, 1.0,  1.0,  -1.0, 1.0,  -1.0};
  processObjFile(CUBE, &vertexIndices, &vertexIndicesCount, &vertices,
                 &vertexCount);
  for (int i = 0; i < 24; i++) {
    ck_assert_float_eq_tol(result[i], vertices[i], 1e-06);
  }
  clean();
}
END_TEST

START_TEST(test_search_for_extremes) {
  float maxValue;
  float minValue;
  processObjFile(SKULL, &vertexIndices, &vertexIndicesCount, &vertices,
                 &vertexCount);
  search_for_extremes(&vertices, &vertexCount, zIndex, &minValue, &maxValue);
  ck_assert_float_eq_tol(1.37702, maxValue, 1e-05);
  ck_assert_float_eq_tol(-4.75944, minValue, 1e-05);
  clean();
}
END_TEST

START_TEST(test_rotate_x) {
  float angle = 120;
  float prev = 140;
  float result[24] = {1.000000,  -1.321027, 0.504863,  1.000000,  0.504863,
                      1.321027,  -1.000000, 0.504863,  1.321027,  -1.000000,
                      -1.321027, 0.504863,  1.000000,  -0.504863, -1.321027,
                      1.000000,  1.321027,  -0.504863, -1.000000, 1.321027,
                      -0.504863, -1.000000, -0.504863, -1.321027};
  processObjFile(CUBE, &vertexIndices, &vertexIndicesCount, &vertices,
                 &vertexCount);
  rotate(&vertices, &vertexCount, xIndex, angle, prev);
  for (int i = 0; i < 24; i++) {
    ck_assert_float_eq_tol(result[i], vertices[i], 1e-06);
  }
  clean();
}
END_TEST

START_TEST(test_rotate_y) {
  float angle = 94;
  float prev = 115;
  float result[24] = {0.288926,  -1.000000, 1.384385,  -1.384385, -1.000000,
                      0.288926,  -0.288926, -1.000000, -1.384385, 1.384385,
                      -1.000000, -0.288926, 0.288926,  1.000000,  1.384385,
                      -1.384385, 1.000000,  0.288926,  -0.288926, 1.000000,
                      -1.384385, 1.384385,  1.000000,  -0.288926};
  processObjFile(CUBE, &vertexIndices, &vertexIndicesCount, &vertices,
                 &vertexCount);
  rotate(&vertices, &vertexCount, yIndex, angle, prev);
  for (int i = 0; i < 24; i++) {
    ck_assert_float_eq_tol(result[i], vertices[i], 1e-06);
  }
  clean();
}
END_TEST

START_TEST(test_rotate_z) {
  float angle = 100;
  float prev = 82;
  float result[24] = {-0.090671, -1.411304, -1.000000, -0.090671, -1.411304,
                      1.000000,  -1.411304, 0.090671,  1.000000,  -1.411304,
                      0.090671,  -1.000000, 1.411304,  -0.090671, -1.000000,
                      1.411304,  -0.090671, 1.000000,  0.090671,  1.411304,
                      1.000000,  0.090671,  1.411304,  -1.000000};
  processObjFile(CUBE, &vertexIndices, &vertexIndicesCount, &vertices,
                 &vertexCount);
  rotate(&vertices, &vertexCount, zIndex, angle, prev);
  for (int i = 0; i < 24; i++) {
    ck_assert_float_eq_tol(result[i], vertices[i], 1e-06);
  }
  clean();
}
END_TEST

START_TEST(test_scale) {
  float value = 4;
  float prev = 2;
  float result[24] = {8.00, -8.00, -8.00, 8.00,  -8.00, 8.00,  -8.00, -8.00,
                      8.00, -8.00, -8.00, -8.00, 8.00,  8.00,  -8.00, 8.00,
                      8.00, 8.00,  -8.00, 8.00,  8.00,  -8.00, 8.00,  -8.00};
  processObjFile(CUBE, &vertexIndices, &vertexIndicesCount, &vertices,
                 &vertexCount);
  scale(&vertices, &vertexCount, value, prev);
  for (int i = 0; i < 24; i++) {
    ck_assert_float_eq_tol(result[i], vertices[i], 1e-06);
  }
  clean();
}
END_TEST

START_TEST(test_move) {
  float value = 3;
  float prev = 2;
  float result[24] = {2.00, -1.00, -1.00, 2.00,  -1.00, 1.00, 0.00,  -1.00,
                      1.00, 0.00,  -1.00, -1.00, 2.00,  1.00, -1.00, 2.00,
                      1.00, 1.00,  0.00,  1.00,  1.00,  0.00, 1.00,  -1.00};
  processObjFile(CUBE, &vertexIndices, &vertexIndicesCount, &vertices,
                 &vertexCount);
  translate(&vertices, &vertexCount, xIndex, value, prev);
  for (int i = 0; i < 24; i++) {
    ck_assert_float_eq_tol(result[i], vertices[i], 1e-06);
  }
  clean();
}
END_TEST

START_TEST(test_check_for_not_opening) {
  int status = processObjFile("../Obj/something/cube.obj", &vertexIndices,
                              &vertexIndicesCount, &vertices, &vertexCount);
  ck_assert_int_eq(status, ERROR);
  clean();
}
END_TEST

Suite *backend_3d_viewer_test(void) {
  Suite *s = suite_create("backend_3d_viewer_test");

  TCase *tc_check_invalid_faces =
      tcase_create("test_check_facets_invalid_input");
  tcase_add_test(tc_check_invalid_faces, test_check_facets_invalid_input);
  suite_add_tcase(s, tc_check_invalid_faces);

  TCase *tc_vertex_valid = tcase_create("test_check_vertex_valid");
  tcase_add_test(tc_vertex_valid, test_check_vertex_valid);
  suite_add_tcase(s, tc_vertex_valid);

  TCase *tc_check_vertex = tcase_create("test_check_vertex");
  tcase_add_test(tc_check_vertex, test_check_vertex);
  suite_add_tcase(s, tc_check_vertex);

  TCase *tc_check_for_extremes = tcase_create("test_search_for_extremes");
  tcase_add_test(tc_check_for_extremes, test_search_for_extremes);
  suite_add_tcase(s, tc_check_for_extremes);

  TCase *tc_test_rotate_x = tcase_create("test_rotate_x");
  tcase_add_test(tc_test_rotate_x, test_rotate_x);
  suite_add_tcase(s, tc_test_rotate_x);

  TCase *tc_test_rotate_y = tcase_create("test_rotate_y");
  tcase_add_test(tc_test_rotate_y, test_rotate_y);
  suite_add_tcase(s, tc_test_rotate_y);

  TCase *tc_test_rotate_z = tcase_create("test_rotate_z");
  tcase_add_test(tc_test_rotate_z, test_rotate_z);
  suite_add_tcase(s, tc_test_rotate_z);

  TCase *tc_test_scale = tcase_create("test_scale");
  tcase_add_test(tc_test_scale, test_scale);
  suite_add_tcase(s, tc_test_scale);

  TCase *tc_test_move = tcase_create("test_move");
  tcase_add_test(tc_test_move, test_move);
  suite_add_tcase(s, tc_test_move);

  TCase *tc_check_for_not_opening = tcase_create("test_check_for_not_opening");
  tcase_add_test(tc_check_for_not_opening, test_check_for_not_opening);
  suite_add_tcase(s, tc_check_for_not_opening);
  return s;
}

int main(void) {
  int number_failed;
  Suite *s;
  SRunner *sr;

  // Инициализация тестового набора
  s = backend_3d_viewer_test();
  sr = srunner_create(s);

  // Запуск всех тестовых наборов
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);

  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
