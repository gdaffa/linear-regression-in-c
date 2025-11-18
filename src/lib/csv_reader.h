#ifndef _CSV_READER
#define _CSV_READER 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "data_type.h"

struct {
   const size_t bufferSize;
} pCSV = { BUFSIZ };

typedef struct CSV {
   float_t** data;
   char**    labels;
   char*     content;
   size_t    rowTotal;
   size_t    colTotal;
   size_t    contentLen;
} CSV;

/**
 * Split column label and calculate total column.
 */
void fCSV_parseLabels(CSV* csv, char* header)
{
   //* property for `csv`
   char** labels   = malloc(0);
   size_t colTotal = 0;

   char* label = strtok(header, ",");
   // loop throught every label and calculate the total column
   while (label != NULL) {
      colTotal++;
      size_t idx      = colTotal - 1;
      size_t labelLen = strlen(label);

      // remove newline
      if (label[labelLen - 1] == '\n') {
         label[labelLen - 1] = '\0';
         labelLen--;
      }

      labels      = realloc(labels, sizeof(char*) * colTotal);
      labels[idx] = malloc(labelLen + 1);
      strncpy(labels[idx], label, labelLen + 1);

      label = strtok(NULL, ",");
   }

   csv->labels   = labels;
   csv->colTotal = colTotal;
}

/**
 * Parse content as a regular string.
 */
void fCSV_parseContent(CSV* csv, FILE* fptr)
{
   size_t contentLen  = 0;
   size_t contentSize = pCSV.bufferSize;
   char   buffer[pCSV.bufferSize];

   //* property for `csv`
   char*  content  = malloc(contentSize);
   size_t rowTotal = 0;

   // loop throught every line and combine it with `content`
   while (fgets(buffer, pCSV.bufferSize, fptr) != NULL) {
      size_t bufferLen     = strlen(buffer);
      size_t newContentLen = contentLen + bufferLen;

      if (newContentLen > contentSize - 1) {
         contentSize <<= 1;
         content = realloc(content, contentSize);
      }

      //? paste from null terminator
      strncpy(content + contentLen, buffer, bufferLen + 1);
      contentLen = newContentLen;
      rowTotal++;
   }

   csv->content    = content;
   csv->contentLen = contentLen;
   csv->rowTotal   = rowTotal;
}

/**
 * Parse the content as a float data.
 */
void fCSV_parseData(CSV* csv)
{
   //? str + '\0' + '\0'
   //? strlen + 2 to prevent strtok that jumps to index '\0' + 1
   char* content = malloc(csv->contentLen + 2);
   strncpy(content, csv->content, csv->contentLen + 2);

   // remove newline
   if (content[csv->contentLen - 1] == '\n') {
      content[csv->contentLen - 1] = '\0';
   }

   //* property for `csv`
   float_t** data = malloc(sizeof(float_t*) * csv->rowTotal);

   char* row         = strtok(content, "\n");
   size_t rowIdx     = 0;
   size_t contentIdx = 0;

   // loop throught every row and parse each column as float_t data
   while (row != NULL) {
      data[rowIdx] = malloc(sizeof(float_t) * csv->colTotal);
      //? added to null terminator + 1
      contentIdx  += strlen(row) + 1;

      char*  col    = strtok(row, ",");
      size_t colIdx = 0;

      while (col != NULL) {
         data[rowIdx][colIdx] = atof(col);
         col = strtok(NULL, ",");
         colIdx++;
      }

      //? jump to index '\0' + 1
      row = strtok(content + contentIdx, "\n");
      rowIdx++;
   }

   csv->data = data;
   free(content);
}

/**
 * Read CSV and return an object of `CSV`.
 */
CSV* readCSV(const char* filename)
{
   FILE* fptr = fopen(filename, "r");
   CSV* csv   = malloc(sizeof(CSV));

   if (fptr == NULL) {
      printf("\"%s\" is not found. Aborting the operation...", filename);
      abort();
   }

   char header[pCSV.bufferSize];
   fgets(header, pCSV.bufferSize, fptr);

   fCSV_parseLabels(csv, header);
   fCSV_parseContent(csv, fptr);
   fCSV_parseData(csv);

   fclose(fptr);
   return csv;
}

/**
 * Deeply destroy CSV object.
 */
void closeCSV(CSV* csv, bool destroyData)
{
   for (size_t i = 0; i < csv->colTotal; i++) {
      free(csv->labels[i]);
   }
   if (destroyData) {
      for (size_t i = 0; i < csv->rowTotal; i++) {
         free(csv->data[i]);
      }
      free(csv->data);
   }

   free(csv->content);
   free(csv->labels);
   free(csv);
}

/**
 * Convert CSV data to matrix.
 */
Matrix* fromCSVtoMatrix(CSV* csv, bool destroy)
{
   Matrix* mtx   = malloc(sizeof(Matrix));
   mtx->data     = csv->data;
   mtx->rowTotal = csv->rowTotal;
   mtx->colTotal = csv->colTotal;

   if (destroy) {
      closeCSV(csv, false);
   }
   return mtx;
}

#endif