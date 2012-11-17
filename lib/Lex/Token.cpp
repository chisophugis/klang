//===--- Token.cpp - --------------------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements Token class.
//
//===----------------------------------------------------------------------===//

#include "klang/Lex/Token.h"

using namespace klang;

/// GetTokPrecedence - Get the precedence of the pending binary operator token.
int Token::GetTokPrecedence() {
  if (!isascii(Kind))
    return -1;

  // Make sure it's a declared binop.
  int TokPrec = BinopPrecedence[Kind];
  if (TokPrec <= 0) return -1;
  return TokPrec;
}

