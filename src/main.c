#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lib/csv_reader.h"
#include "lib/lin_reg.h"

typedef struct Option {
   char* short_;
   char* long_;
   bool required;
} Option;

void exitInvalidArgs()
{
   puts("Arguments are invalid, aborting the operation...");
   exit(1);
}

int main(int argc, char* argv[])
{
   char* filename;
   size_t target = 0;

   Option opt[] = {
      {"-d", "--data", true},
      {"-t", "--target", true},
      {"-r", "--learning-rate", false},
      {"-s", "--max-step-size", false}
   };
   size_t optSize = sizeof(opt) / sizeof(Option);

   for (size_t argIdx = 1; argIdx < argc;) {
      char* optVal;
      char  optShort = 0;

      for (size_t optIdx = 0; optIdx < optSize; optIdx++) {
         // if the argument match and next argument is available
         if (
            strcmp(argv[argIdx], opt[optIdx].short_) == 0
            || strcmp(argv[argIdx], opt[optIdx].long_) == 0
            && argIdx + 1 < argc
         ) {
            opt[optIdx].required = false;
            optShort = opt[optIdx].short_[1];
            optVal   = argv[argIdx + 1];

            // option (current) and value (next) is used,
            // so we jump/start to the index after next
            argIdx += 2;
            break;
         }
      }

      switch (optShort) {
         case 'd':
            filename = optVal;
            break;
         case 't':
            target = strtol(optVal, NULL, 10);
            break;
         case 'r':
            pLinReg_hyperParam.learningRate = strtod(optVal, NULL);
            break;
         case 's':
            pLinReg_hyperParam.maxStep = strtod(optVal, NULL);
            break;
         default:
            exitInvalidArgs();
            break;
      }
   }

   // if the required options are missing
   for (size_t i = 0; i < optSize; i++) {
      if (opt[i].required) {
         exitInvalidArgs();
      }
   }

   CSV* csv = readCSV(filename);
   LinRegResult* lr = calcLinReg(fromCSVtoMatrix(csv, false), target - 1);
   printLinRegResult(stdout, lr);

   closeLinRegResult(lr);
   closeCSV(csv, true);
   return 0;
}