//===--- Lexer.cpp - --------------------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements lexer class.
//
//===----------------------------------------------------------------------===//

#include "klang/Driver/Driver.h"
#include "klang/Lex/Lexer.h"

using namespace klang;

/// Return the next token from standard input.
void
Lexer::Lex(Token &Result) {

  // Skip any whitespace.
  while (isspace(LastChar))
    LastChar = GetCharFromBuffer();

  if (isalpha(LastChar)) { // identifier: [a-zA-Z][a-zA-Z0-9]*
    Result.IdentifierStr = LastChar;
    while (isalnum((LastChar = GetCharFromBuffer())))
      Result.IdentifierStr += LastChar;

    if (Result.IdentifierStr == "def") {
      Result.Kind = tok::tok_def; return;
    }
    if (Result.IdentifierStr == "extern") {
      Result.Kind = tok::tok_extern; return;
    }
    if (Result.IdentifierStr == "if") {
      Result.Kind = tok::tok_if; return;
    }
    if (Result.IdentifierStr == "then") {
      Result.Kind = tok::tok_then; return;
    }
    if (Result.IdentifierStr == "else") {
      Result.Kind = tok::tok_else; return;
    }
    if (Result.IdentifierStr == "for") {
      Result.Kind = tok::tok_for; return;
    }
    if (Result.IdentifierStr == "in") {
      Result.Kind = tok::tok_in; return;
    }
    if (Result.IdentifierStr == "binary") {
      Result.Kind = tok::tok_binary; return;
    }
    if (Result.IdentifierStr == "unary") {
      Result.Kind = tok::tok_unary; return;
    }
    if (Result.IdentifierStr == "var") {
      Result.Kind = tok::tok_var; return;
    }

    Result.Kind = tok::tok_identifier;
    return;
  }

  if (isdigit(LastChar) || LastChar == '.') {   // Number: [0-9.]+
    std::string NumStr;
    do {
      NumStr += LastChar;
      LastChar = GetCharFromBuffer();
    } while (isdigit(LastChar) || LastChar == '.');

    Result.NumVal = strtod(NumStr.c_str(), 0);

    Result.Kind  = tok::tok_number;
    return;
  }

  if (LastChar == '#') {
    // Comment until end of line.
    do LastChar = GetCharFromBuffer();
    while (LastChar != KLANG_LEXER_EOF && LastChar != '\n' && LastChar != '\r');

    if (LastChar != KLANG_LEXER_EOF) {
      Lex(Result);
      return;
    }
  }

  // Check for end of file.  Don't eat the KLANG_LEXER_EOF.
  if (LastChar == KLANG_LEXER_EOF) {
    Result.Kind = tok::tok_eof;
    return;
  }

  // Otherwise, just return the character as its ascii value.
  int ThisChar = LastChar;
  LastChar = GetCharFromBuffer();
  Result.Kind = ThisChar;
  return;
}


int
Lexer::GetCharFromBuffer(void)
{
  static int index = 0;

  //returns KLANG_LEXER_EOF for the character after the last one in the Buffer
  return ((unsigned)(index+1) <= Buffer.size()) ?
    (int)Buffer[index++] : KLANG_LEXER_EOF;
}


Lexer::Lexer(llvm::StringRef _Buffer)
  : LastChar(' '), Buffer(_Buffer)
{
}


