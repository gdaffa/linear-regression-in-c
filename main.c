#include <stdio.h>
#include "csv_reader.h"

int main()
{
   CSV* csv   = readCSV("input.csv");
   FILE* fout = fopen("output.csv", "w");

   // header, csv column name
   for (size_t col = 0; col < csv->colTotal; col++) {
      fprintf(fout, col == 0 ? "%s" : ",%s", csv->labels[col]);
   }
   fprintf(fout, "\n");

   // main content, csv data
   for (size_t row = 0; row < csv->rowTotal; row++) {
      for (size_t col = 0; col < csv->colTotal; col++) {
         fprintf(fout, col == 0 ? "%.3f" : ",%.3f", csv->data[row][col] * 5);
      }
      fprintf(fout, "\n");
   }

   fclose(fout);
   closeCSV(csv, true);
}