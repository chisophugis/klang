//===--- Lexer.h - ----------------------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines lexer class.
//
//===----------------------------------------------------------------------===//

#ifndef KLANG_LEXER_H
#define KLANG_LEXER_H

#include "klang/Lex/Token.h"

namespace klang {

  class Lexer {

    int LastChar;

  public:
    Lexer();

    /// Return the next token from standard input.
    void Lex(Token &Result);

  };

}

#endif //#ifndef KLANG_LEXER_H