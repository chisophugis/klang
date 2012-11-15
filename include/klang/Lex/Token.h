#ifndef KLANG_TOKEN_H
#define KLANG_TOKEN_H

#include <string>
#include <map>
#include "klang/Lex/TokenKinds.h"

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

