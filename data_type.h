#ifndef _DATA_TYPE_H
#define _DATA_TYPE_H

/**
 * Used to determine `float_t` and `double_t` data type, defined in `math.h`.
 * 0: `float` & `double`
 * 1: both `double`
 * else: both `long double`
 */
#undef __FLT_EVAL_METHOD__
#define __FLT_EVAL_METHOD__ 0

#include <stdio.h>
#include <stdbool.h>
#include <math.h>

typedef struct Matrix {
   float_t** data;
   size_t    rowTotal;
   size_t    colTotal;
} Matrix;

#endif