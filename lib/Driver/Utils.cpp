
#include "klang/Driver/Utils.h"
#include "llvm/IR/Module.h"
#include <cstdio>

namespace klang {

  //Error* - These are little helper functions for error handling.
  ExprAST *Error(const char *Str) {
    fprintf(stderr, "Error: %s\n", Str);
    return 0;
  }

  PrototypeAST *ErrorP(const char *Str) {
    Error(Str);
    return 0;
  }

  FunctionAST *ErrorF(const char *Str) {
    Error(Str);
    return 0;
  }

  llvm::Value *ErrorV(const char *Str) {
    Error(Str);
    return 0;
  }

}

