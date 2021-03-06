//===--- Utils.cpp - --------------------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements utility functions.
//
//===----------------------------------------------------------------------===//

#include "klang/Driver/Utils.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"
#include <cstdio>


namespace klang {

  //Error* - These are little helper functions for error handling.
  ExprAST *Error(const char *Str) {
    llvm::errs() << "Error: " << Str << "\n";
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

