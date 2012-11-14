#include "klang/AST/ASTNodes.h"
#include "klang/Lex/Lexer.h"
#include "klang/Parse/Parser.h"

using namespace klang;

//===----------------------------------------------------------------------===//
// Main driver code.
//===----------------------------------------------------------------------===//

std::map<char, int> Token::BinopPrecedence;

int main() {

  Lexer myLexer;
  Parser myParser(myLexer);

  // Install standard binary operators.
  // 1 is lowest precedence.
  Token::BinopPrecedence['<'] = 10;
  Token::BinopPrecedence['+'] = 20;
  Token::BinopPrecedence['-'] = 20;
  Token::BinopPrecedence['*'] = 40;  // highest.

  // Run the main "interpreter loop" now.
  myParser.Go();

  return 0;
}

