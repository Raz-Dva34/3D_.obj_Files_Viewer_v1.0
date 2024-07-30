#ifndef TEST_H
#define TEST_H

#include <check.h>
#include <stdio.h>
#include <stdlib.h>

#include "../backend/readobj.c"
#include "../backend/readobj.h"

#define CUBE "Obj/cube.obj"
#define SKULL "Obj/skull.obj"

unsigned int *vertexIndices = NULL;
unsigned int vertexIndicesCount = 0;
float *vertices = NULL;
unsigned int vertexCount = 0;

void clean() {
  free(vertices);
  free(vertexIndices);
  vertexCount = 0;
  vertexIndicesCount = 0;
}

#endif /* TEST_H */
