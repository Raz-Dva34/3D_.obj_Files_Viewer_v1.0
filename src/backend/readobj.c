#include "readobj.h"

void checkSlashesError(char *token, int *status) {
  int countSlashes = 0;

  for (unsigned int i = 0; token[i] != '\0'; i++) {
    if (token[i] == '/') countSlashes++;
  }
  if (countSlashes != 2 && countSlashes != 0) {
    printf("Error: Invalid vertex index format: %s\n", token);
    *status = ERROR;
  }
}
void fileOpenCheck(FILE *file, int *status, const char *filename) {
  if (!file) {
    printf("Could not open file %s for reading.\n", filename);
    *status = ERROR;
  }
}

int hasDigits(char *str) {
  int status = ERROR;
  while (*str) {
    if (isdigit(*str)) status = OK;
    str++;
  }
  return status;
}

int processObjFile(const char *filename, unsigned int **vertexIndices,
                   unsigned int *vertexIndicesCount, float **vertices,
                   unsigned int *vertexCount) {
  int status = OK;
  FILE *file = fopen(filename, "r");
  fileOpenCheck(file, &status, filename);
  if (status == OK) {
    char line[256];
    *vertexIndicesCount = 0;
    while (fgets(line, sizeof(line), file) && status == OK) {
      if (line[0] == 'v' && line[1] == ' ') {
        if (status == OK) {
          (*vertexCount) += 3;
          *vertices =
              (float *)realloc(*vertices, (*vertexCount) * sizeof(float));
          sscanf(line, "v %f %f %f", &(*vertices)[((*vertexCount) - 3)],
                 &(*vertices)[((*vertexCount) - 2)],
                 &(*vertices)[((*vertexCount) - 1)]);
        }
      } else if (line[0] == 'f' && line[1] == ' ') {
        char *token = strtok(line + 2, " \n");
        while (token) {
          if (!hasDigits(token)) {
            checkSlashesError(token, &status);
            int vertexIndex = atoi(token);
            *vertexIndices = (unsigned int *)realloc(
                *vertexIndices,
                (*(vertexIndicesCount) + 1) * sizeof(unsigned int));
            if (vertexIndex == 0) {
              status = ERROR;
              printf("Error: Invalid vertex index format: %d\n", vertexIndex);
            } else if (vertexIndex < 0)
              (*vertexIndices)[*vertexIndicesCount] =
                  (unsigned int)((*vertexCount) / 3 + vertexIndex);
            else
              (*vertexIndices)[*vertexIndicesCount] =
                  (unsigned int)(vertexIndex - 1);
            (*vertexIndicesCount)++;
          }
          token = strtok(NULL, " \n");
        }
      }
    }
    fclose(file);
  }
  ft_rewrite(vertexIndices, vertexIndicesCount);
  return status;
}

void ft_rewrite(unsigned int **vertexIndices,
                unsigned int *vertexIndicesCount) {
  unsigned int *arrayForOpenGL =
      (unsigned int *)calloc((*vertexIndicesCount) * 2, sizeof(unsigned int));

  for (unsigned int i = 0; i < (*vertexIndicesCount); i += 3) {
    arrayForOpenGL[i * 2] = (*vertexIndices)[i];
    arrayForOpenGL[(i * 2) + 1] = (*vertexIndices)[i + 1];
    arrayForOpenGL[(i * 2) + 2] = (*vertexIndices)[i + 1];
    arrayForOpenGL[(i * 2) + 3] = (*vertexIndices)[i + 2];
    arrayForOpenGL[(i * 2) + 4] = (*vertexIndices)[i + 2];
    arrayForOpenGL[(i * 2) + 5] = (*vertexIndices)[i];
  }

  *vertexIndicesCount *= 2;

  free(*vertexIndices);

  *vertexIndices =
      (unsigned int *)calloc(*vertexIndicesCount, sizeof(unsigned int));

  for (unsigned int i = 0; i < *vertexIndicesCount; i++) {
    (*vertexIndices)[i] = arrayForOpenGL[i];
  }

  free(arrayForOpenGL);
}

void translate(float **vertices, unsigned int *vertexCount, int coordinateIndex,
               int value, int prev) {
  for (unsigned int i = 0; i < *vertexCount; i += 3) {
    (*vertices)[i + coordinateIndex] += (value - prev);
  }
}

void scale(float **vertices, unsigned int *vertexCount, float value,
           float prev) {
  for (unsigned int i = 0; i < *vertexCount; i++) {
    (*vertices)[i] *= value * prev;
  }
}

long double angle_normalize(long double angle) {
  return (angle * 3.14 / 180.0);
}

void rotate(float **vertices, unsigned int *vertexCount, int coordinateIndex,
            float angle, float prev) {
  if (coordinateIndex == 0) {  // Поворот по оси X
    for (unsigned int i = 0; i < *vertexCount; i += 3) {
      float tmpY = (*vertices)[i + 1];
      float tmpZ = (*vertices)[i + 2];
      (*vertices)[i + 1] = cos(angle - prev) * tmpY - sin(angle - prev) * tmpZ;
      (*vertices)[i + 2] = sin(angle - prev) * tmpY + cos(angle - prev) * tmpZ;
    }
  } else if (coordinateIndex == 1) {  // Поворот по оси Y
    for (unsigned int i = 0; i < *vertexCount; i += 3) {
      float tmpX = (*vertices)[i];
      float tmpZ = (*vertices)[i + 2];
      (*vertices)[i] = cos(angle - prev) * tmpX + sin(angle - prev) * tmpZ;
      (*vertices)[i + 2] = -sin(angle - prev) * tmpX + cos(angle - prev) * tmpZ;
    }
  } else {  // Поворот по оси Z
    for (unsigned int i = 0; i < *vertexCount; i += 3) {
      float tmpX = (*vertices)[i];
      float tmpY = (*vertices)[i + 1];
      (*vertices)[i] = cos(angle - prev) * tmpX - sin(angle - prev) * tmpY;
      (*vertices)[i + 1] = sin(angle - prev) * tmpX + cos(angle - prev) * tmpY;
    }
  }
}

void search_for_extremes(float **vertices, unsigned int *vertexCount,
                         unsigned int coordinateIndex, float *minValue,
                         float *maxValue) {
  (*maxValue) = -FLT_MAX;
  (*minValue) = FLT_MAX;
  for (unsigned int i = 0; i < *(vertexCount); i += 3) {
    if ((*maxValue) < (*vertices)[i + coordinateIndex])
      (*maxValue) = (*vertices)[i + coordinateIndex];
    if ((*minValue) > (*vertices)[i + coordinateIndex])
      (*minValue) = (*vertices)[i + coordinateIndex];
  }
}

void searchExtreme(float **vertices, unsigned int *vertexCount,
                   float *maxValue) {
  (*maxValue) = -FLT_MAX;
  for (unsigned int i = 0; i < *(vertexCount); i++) {
    if ((*maxValue) < (*vertices)[i]) (*maxValue) = (*vertices)[i];
  }
}

// int main() {
//   float *vertices = NULL;
//   unsigned int vertexCount = 0;
//   unsigned int *vertexIndices = NULL;
//   unsigned int vertexIndicesCount = 0;
//   processObjFile("../Obj/cube.obj", &vertexIndices, &vertexIndicesCount,
//   &vertices,
//                  &vertexCount);
//   printf("%d\n", vertexCount);
//
//   for (unsigned int i = 0; i < vertexCount; i++) {
//     printf("%f  ", vertices[i]);
//   }
//   float maxValue, minValue;
//   search_for_extremes(
//       &vertices, &vertexCount, zIndex, &minValue,
//       &maxValue); /*ПОИСК МАКСИМАЛЬНОГО И МИНИМАЛЬНОГО ЗНАЧЕНИЯ*/
//   printf("\nмаксимальное: %f     минимальное: %f\n", maxValue, minValue);
//   printf("\n%d\n", vertexIndicesCount);
////   printf("\n%d\n", vertexIndicesCount);
////   for (int i = 0; i < vertexIndicesCount * 2; i++) {
////     printf("%d, ", arrayForOpenGL[i]);
////   }
//   printf("\n");
//   rotate(&vertices, &vertexCount, zIndex, 100, 82);
//  for (unsigned int i = 0; i < vertexCount; i++) {
//    printf("%f,  ", vertices[i]);
//  }
//    free(vertices);
//    free(vertexIndices);
//  return (0);
//}
