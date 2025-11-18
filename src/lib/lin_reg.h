#ifndef _LIN_REG_H
#define _LIN_REG_H

#include <stdlib.h>
#include "data_type.h"
#include "csv_reader.h"

typedef struct LinRegResult {
   float_t* slope;
   float_t  intercept;
   size_t   slopeSize;
} LinRegResult;

struct {
   float_t maxStep;
   float_t learningRate;
   float_t iterMax;
} pLinReg_hyperParam = {
   1.0E-6, 1.0E-5, 1000000
};

/**
 * Split the data to feature and target data.
 */
void fLinReg_splitFeatTarg(Matrix* mtx, float_t** featData, float_t* targData, size_t targCol)
{
   for (size_t rowIdx = 0; rowIdx < mtx->rowTotal; rowIdx++) {
      featData[rowIdx]  = (float_t*) malloc(sizeof(float_t*) * mtx->colTotal);
      size_t featColIdx = 0;

      for (size_t colIdx = 0; colIdx < mtx->colTotal; colIdx++) {
         if (colIdx == targCol) {
            targData[rowIdx] = mtx->data[rowIdx][colIdx];
            continue;
         }

         featData[rowIdx][colIdx] = mtx->data[rowIdx][colIdx];
         featColIdx++;
      }
   }
}

/**
 * Free feature and target data after linear regression calculation.
 */
void fLinReg_freeFeatTarg(Matrix* mtx, float_t** featData, float_t* targData)
{
   for (size_t rowIdx = 0; rowIdx < mtx->rowTotal; rowIdx++) {
      free(featData[rowIdx]);
   }
   free(featData);
   free(targData);
}

/**
 * The loss function.
 * Using the derivative of squared residual.
 *
 * Residual formula is
 * => `(y - y_pred)^2`
 * => `(y - (a*x + b))^2`
 *
 * As we know the chain rule that
 * `(f(g(x)))' = f'(g(x)) * g'(x)`
 *
 * => `g(x) = residual = y - (a*x + b)`
 * => `f(g(x)) = residual^2 = g(x)^2`
 *
 * So the derivative is
 * `(2 * (y - (a*x + b))) * (derivative(y - (a*x + b)))`
 */
float_t fLinReg_derv(float_t derv1, float_t respectTo)
{
   return derv1 * -respectTo;
}

/**
 * Calculate the 1st derivative of the chain rule.
 *
 * => `f(g(x)) = residual^2 = (y - (a*x + b))^2`
 * => `f'(g(x)) = 2 * residual = 2 * (y - (a*x + b))`
 *
 * See more on `fLinReg_derv` function.
 */
float_t fLinReg_derv1(LinRegResult* var, float_t* featDataCol, float_t y)
{
   float_t y_pred = var->intercept;
   for (size_t n = 0; n < var->slopeSize; n++) {
      y_pred += featDataCol[n] * var->slope[n];
   }

   return 2 * (y - y_pred);
}

/**
 * Evaluate if the absolute step has reached the maximum step.
 */
bool fLinReg_evalStep(float_t step)
{
   return step < pLinReg_hyperParam.maxStep
      && step > -pLinReg_hyperParam.maxStep;
}

/**
 * Update the intercept and slope, and check if all step is small.
 */
bool fLinReg_gradDesc(LinRegResult* var, float_t sumDervIntercept, float_t* sumDervSlope)
{
   float_t step;
   bool    isStepSmall = true;

   step = sumDervIntercept * pLinReg_hyperParam.learningRate;
   var->intercept -= step;
   isStepSmall &= fLinReg_evalStep(step);

   for (size_t slopeIdx = 0; slopeIdx < var->slopeSize; slopeIdx++) {
      step = sumDervSlope[slopeIdx] * pLinReg_hyperParam.learningRate;
      var->slope[slopeIdx] -= step;
      isStepSmall &= fLinReg_evalStep(step);
   }

   return isStepSmall;
}

/**
 * The main function to calculate linear regression using gradient descent.
 *
 * See `pLinReg_hyperParam` to modify and check the hyper parameter.
 */
LinRegResult* calcLinReg(Matrix* mtx, size_t targCol)
{
   LinRegResult* var = (LinRegResult*) calloc(1, sizeof(LinRegResult));

   size_t slopeSize = mtx->colTotal - 1;

   var->slopeSize = slopeSize;
   var->slope     = (float_t*) calloc(slopeSize, sizeof(float_t));
   var->intercept = 0;

   for (size_t slopeIdx = 0; slopeIdx < slopeSize; slopeIdx++) {
      var->slope[slopeIdx] = slopeIdx + 1;
   }

   float_t** featData = (float_t**) malloc( sizeof(float_t*) * mtx->rowTotal);
   float_t*  targData = (float_t*) malloc( sizeof(float_t) * mtx->rowTotal);

   fLinReg_splitFeatTarg(mtx, featData, targData, targCol);

   size_t i;
   for (i = 0; i < pLinReg_hyperParam.iterMax; i++) {
      float_t sumDervIntercept = 0;
      float_t sumDervSlope[slopeSize];

      for (size_t slopeIdx = 0; slopeIdx < slopeSize; slopeIdx++) {
         sumDervSlope[slopeIdx] = 0;
      }

      for (size_t rowIdx = 0; rowIdx < mtx->rowTotal; rowIdx++) {
         float_t derv1 = fLinReg_derv1(
            var, featData[rowIdx], targData[rowIdx]
         );

         // derivative that respect to the intercept is always 1
         sumDervIntercept += fLinReg_derv(derv1, 1);

         for (size_t slopeIdx = 0; slopeIdx < slopeSize; slopeIdx++) {
            // derivative that respect to the slope is always `x` variable
            sumDervSlope[slopeIdx] += fLinReg_derv(
               derv1, featData[rowIdx][slopeIdx]
            );
         }
      }

      bool isStepSmall = fLinReg_gradDesc(
         var, sumDervIntercept, sumDervSlope
      );
      if (isStepSmall) {
         break;
      }
   }
   fLinReg_freeFeatTarg(mtx, featData, targData);

   if (i == pLinReg_hyperParam.iterMax) {
      puts("Operation reached the maximum iteration. The result might be bad.");
   }

   return var;
}

/**
 * Destroy `LineRegResult` object.
 */
void closeLinRegResult(LinRegResult* var)
{
   free(var->slope);
   free(var);
}

/**
 * Print the `LinRegResult` result to a file in algebra notation.
 */
void printLinRegResult(FILE* fptr, LinRegResult* result)
{
   char var = result->slopeSize > 1 ? 'a' : 'x';
   fprintf(fptr, "%lf", result->intercept);
   for (size_t i = 0; i < result->slopeSize; i++) {
      fprintf(fptr, " + %lf%c", result->slope[i], var + i);
   }
   fprintf(fptr, "\n");
}

#endif