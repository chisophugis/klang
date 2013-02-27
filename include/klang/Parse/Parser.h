//===--- Parser.h - ---------------------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file defines the Parser class.
///
//===----------------------------------------------------------------------===//

#ifndef KLANG_PARSER_H
#define KLANG_PARSER_H

#include "klang/AST/ASTNodes.h"
#include "klang/Lex/Lexer.h"

namespace klang {

  class Parser {

    Lexer &Lxr;

    // Tok - The current token we are peeking ahead.  All parsing methods assume
    // that this is valid.
    Token Tok;

  public:
    Parser(Lexer &_Lxr)
      : Lxr(_Lxr)
    {}

    int GetNextToken();

    ExprAST *ParseIdentifierExpr();
    ExprAST *ParseNumberExpr();
    ExprAST *ParseParenExpr();
    ExprAST *ParseIfExpr();
    ExprAST *ParseForExpr();
    ExprAST *ParseVarExpr();
    ExprAST *ParsePrimary();
    ExprAST *ParseUnary();
    ExprAST *ParseBinOpRHS(int ExprPrec, ExprAST *LHS);
    ExprAST *ParseExpression();
    PrototypeAST *ParsePrototype();
    FunctionAST *ParseDefinition();
    FunctionAST *ParseTopLevelExpr();
    PrototypeAST *ParseExtern();

    //===------------------------------------------------------------------===//
    // Top-Level parsing
    //===------------------------------------------------------------------===//

    void HandleDefinition();
    void HandleExtern();
    void HandleTopLevelExpression();
    /// top ::= definition | external | expression | ';'
    void Go();

  };

}

#endif //#ifndef KLANG_PARSER_H

