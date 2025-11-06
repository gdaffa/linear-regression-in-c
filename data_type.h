#ifndef _DATA_TYPE_H
#define _DATA_TYPE_H

#include <stdio.h>
#include <stdbool.h>

typedef struct Matrix {
   float** data;
   size_t rowTotal;
   size_t colTotal;
} Matrix;

#endif