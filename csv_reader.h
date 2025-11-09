#ifndef _CSV_READER
#define _CSV_READER 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "data_type.h"

struct {
   const size_t bufferSize;
} CSVProps = { BUFSIZ };

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
void parseLabels(CSV* csv, char* header)
{
   //* property for `csv`
   char** labels   = (char**) malloc(0);
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

      labels      = (char**) realloc(labels, sizeof(char*) * colTotal);
      labels[idx] = (char*) calloc(labelLen + 1, 1);
      strncpy(labels[idx], label, labelLen + 1);

      label = strtok(NULL, ",");
   }

   csv->labels   = labels;
   csv->colTotal = colTotal;
}

/**
 * Parse content as a regular string.
 */
void parseContent(CSV* csv, FILE* fptr)
{
   size_t contentLen  = 0;
   size_t contentSize = CSVProps.bufferSize;
   char   buffer[CSVProps.bufferSize];

   //* property for `csv`
   char*  content  = (char*) calloc(contentSize, 1);
   size_t rowTotal = 0;

   // loop throught every line and combine it with `content`
   while (fgets(buffer, CSVProps.bufferSize, fptr) != NULL) {
      size_t bufferLen     = strlen(buffer);
      size_t newContentLen = contentLen + bufferLen;

      if (newContentLen > contentSize - 1) {
         contentSize <<= 1;
         content = (char*) realloc(content, contentSize);
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
void parseData(CSV* csv)
{
   //? str + '\0' + '\0'
   //? strlen + 2 to prevent strtok that jumps to index '\0' + 1
   char* content = (char*) calloc(csv->contentLen + 2, 1);
   strncpy(content, csv->content, csv->contentLen + 2);

   // remove newline
   if (content[csv->contentLen - 1] == '\n') {
      content[csv->contentLen - 1] = '\0';
   }

   //* property for `csv`
   float_t** data = (float_t**) calloc(csv->rowTotal, sizeof(float_t*));

   char* row         = strtok(content, "\n");
   size_t rowIdx     = 0;
   size_t contentIdx = 0;

   // loop throught every row and parse each column as float_t data
   while (row != NULL) {
      data[rowIdx] = (float_t*) malloc(csv->colTotal * sizeof(float_t));
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
   CSV* csv   = (CSV*) malloc(sizeof(CSV));

   if (fptr == NULL) {
      printf("\"%s\" is not found. Aborting the operation...", filename);
      abort();
   }

   char header[CSVProps.bufferSize];
   fgets(header, CSVProps.bufferSize, fptr);

   parseLabels(csv, header);
   parseContent(csv, fptr);
   parseData(csv);

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
   Matrix* mtx   = calloc(1, sizeof(Matrix));
   mtx->data     = csv->data;
   mtx->rowTotal = csv->rowTotal;
   mtx->colTotal = csv->colTotal;

   if (destroy) {
      closeCSV(csv, false);
   }
   return mtx;
}

#endif