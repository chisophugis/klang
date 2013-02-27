//===--- Token.h - ----------------------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file defines Token class.
///
//===----------------------------------------------------------------------===//

#ifndef KLANG_TOKEN_H
#define KLANG_TOKEN_H

#include "klang/Lex/TokenKinds.h"
#include <map>
#include <string>

namespace klang {

  class Token {
    friend class Lexer;
    friend class Parser;

    int Kind;

    std::string IdentifierStr;  // Filled in if tok_identifier
    double NumVal;              // Filled in if tok_number


    /// GetTokPrecedence - Get the precedence of the pending binary operator
    /// token.
    int GetTokPrecedence();

  public:

    /// BinopPrecedence - This holds the precedence for each binary operator
    /// that is defined.
    static std::map<char, int> BinopPrecedence;
  };

}

#endif //#ifndef KLANG_TOKEN_H

