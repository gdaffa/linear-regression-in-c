#include <stdio.h>
#include "lib/csv_reader.h"
#include "lib/lin_reg.h"

int main(int argc, char* argv[])
{
   if (argc < 3) {
      puts("The argument must be <csv-file> <target-column>.");
      return 1;
   }

   CSV* csv = readCSV(argv[1]);
   LinRegResult* lr = calcLinReg(fromCSVtoMatrix(csv, false), atoi(argv[2]) - 1);
   printLinRegResult(stdout, lr);

   closeLinRegResult(lr);
   closeCSV(csv, true);
   return 0;
}