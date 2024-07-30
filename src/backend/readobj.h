#ifndef READOBJ_H
#define READOBJ_H

#include <ctype.h>
#include <float.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define OK 0
#define ERROR 1

#define xIndex 0
#define yIndex 1
#define zIndex 2
//#define FLT_MAX 3.402823466e+38F

int processObjFile(const char *filename, unsigned int **vertexIndices,
                   unsigned int *vertexIndicesCount, float **vertices,
                   unsigned int *vertexCount);
void translate(float **vertices, unsigned int *vertexCount, int coordinateIndex,
               int value, int prev);
void rotate(float **vertices, unsigned int *vertexCount, int coordinateIndex,
            float angle, float prev);
void scale(float **vertices, unsigned int *vertexCount, float value,
           float prev);
void search_for_extremes(float **vertices, unsigned int *vertexCount,
                         unsigned int coordinateIndex, float *minValue,
                         float *maxValue);

void checkSlashesError(char *token, int *status);
void fileOpenCheck(FILE *file, int *status, const char *filename);
int hasDigits(char *str);
void ft_rewrite(unsigned int **vertexIndices, unsigned int *vertexIndicesCount);
void searchExtreme(float **vertices, unsigned int *vertexCount,
                   float *maxValue);
long double angle_normalize(long double angle);

#endif  // READOBJ_H
