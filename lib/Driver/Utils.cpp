
#include "klang/Driver/Utils.h"
#include <cstdio>


//Error* - These are little helper functions for error handling.
klang::ExprAST *klang::Error(const char *Str) {
  fprintf(stderr, "Error: %s\n", Str);
  return 0;
}

klang::PrototypeAST *klang::ErrorP(const char *Str) {
  Error(Str);
  return 0;
}

klang::FunctionAST *klang::ErrorF(const char *Str) {
  Error(Str);
  return 0;
}


