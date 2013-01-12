//===--- Utils.h - ----------------------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines utility functions.
//
//===----------------------------------------------------------------------===//

#ifndef KLANG_UTIL_H
#define KLANG_UTIL_H

#include "klang/AST/ASTNodes.h"
#include "llvm/IR/Module.h"

namespace klang {

  //Error* - These are little helper functions for error handling.
  ExprAST *Error(const char *Str);
  PrototypeAST *ErrorP(const char *Str);
  FunctionAST *ErrorF(const char *Str);
  llvm::Value *ErrorV(const char *Str);
}
#endif //#ifndef KLANG_UTIL_H

