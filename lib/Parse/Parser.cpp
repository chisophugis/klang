
#include "klang/Driver/Driver.h"
#include "klang/Driver/Utils.h"
#include "klang/Parse/Parser.h"
#include "llvm/Function.h"
#include <cstdio>

using namespace klang;

int Parser::GetNextToken() {
  Lxr.Lex(Tok);
  return Tok.Kind;
}

/// identifierexpr
///   ::= identifier
///   ::= identifier '(' expression* ')'
ExprAST *Parser::ParseIdentifierExpr() {
  std::string IdName = Tok.IdentifierStr;

  GetNextToken();  // eat identifier.

  if (Tok.Kind != '(') // Simple variable ref.
    return new VariableExprAST(IdName);

  // Call.
  GetNextToken();  // eat (
  std::vector<ExprAST*> Args;
  if (Tok.Kind != ')') {
    while (1) {
      ExprAST *Arg = ParseExpression();
      if (!Arg) return 0;
      Args.push_back(Arg);

      if (Tok.Kind == ')') break;

      if (Tok.Kind != ',')
        return Error("Expected ')' or ',' in argument list");
      GetNextToken();
    }
  }

  // Eat the ')'.
  GetNextToken();

  return new CallExprAST(IdName, Args);
}



/// numberexpr ::= number
ExprAST *Parser::ParseNumberExpr() {
  ExprAST *Result = new NumberExprAST(Tok.NumVal);
  GetNextToken(); // consume the number
  return Result;
}

/// parenexpr ::= '(' expression ')'
ExprAST *Parser::ParseParenExpr() {
  GetNextToken();  // eat (.
  ExprAST *V = ParseExpression();
  if (!V) return 0;

  if (Tok.Kind != ')')
    return Error("expected ')'");
  GetNextToken();  // eat ).
  return V;
}

/// ifexpr ::= 'if' expression 'then' expression 'else' expression
ExprAST *Parser::ParseIfExpr() {
  GetNextToken();  // eat the if.

  // condition.
  ExprAST *Cond = ParseExpression();
  if (!Cond) return 0;

  if (Tok.Kind != tok::tok_then)
    return Error("expected then");
  GetNextToken();  // eat the then

  ExprAST *Then = ParseExpression();
  if (Then == 0) return 0;

  if (Tok.Kind != tok::tok_else)
    return Error("expected else");

  GetNextToken();

  ExprAST *Else = ParseExpression();
  if (!Else) return 0;

  return new IfExprAST(Cond, Then, Else);
}

/// forexpr ::= 'for' identifier '=' expr ',' expr (',' expr)? 'in' expression
ExprAST *Parser::ParseForExpr() {
  GetNextToken();  // eat the for.

  if (Tok.Kind != tok::tok_identifier)
    return Error("expected identifier after for");

  std::string IdName = Tok.IdentifierStr;
  GetNextToken();  // eat identifier.

  if (Tok.Kind != '=')
    return Error("expected '=' after for");
  GetNextToken();  // eat '='.


  ExprAST *Start = ParseExpression();
  if (Start == 0) return 0;
  if (Tok.Kind != ',')
    return Error("expected ',' after for start value");
  GetNextToken();

  ExprAST *End = ParseExpression();
  if (End == 0) return 0;

  // The step value is optional.
  ExprAST *Step = 0;
  if (Tok.Kind == ',') {
    GetNextToken();
    Step = ParseExpression();
    if (Step == 0) return 0;
  }

  if (Tok.Kind != tok::tok_in)
    return Error("expected 'in' after for");
  GetNextToken();  // eat 'in'.

  ExprAST *Body = ParseExpression();
  if (Body == 0) return 0;

  return new ForExprAST(IdName, Start, End, Step, Body);
}

/// varexpr ::= 'var' identifier ('=' expression)?
//                    (',' identifier ('=' expression)?)* 'in' expression
ExprAST *Parser::ParseVarExpr() {
  GetNextToken();  // eat the var.

  std::vector<std::pair<std::string, ExprAST*> > VarNames;

  // At least one variable name is required.
  if (Tok.Kind != tok::tok_identifier)
    return Error("expected identifier after var");

  while (1) {
    std::string Name = Tok.IdentifierStr;
    GetNextToken();  // eat identifier.

    // Read the optional initializer.
    ExprAST *Init = 0;
    if (Tok.Kind == '=') {
      GetNextToken(); // eat the '='.

      Init = ParseExpression();
      if (Init == 0) return 0;
    }

    VarNames.push_back(std::make_pair(Name, Init));

    // End of var list, exit loop.
    if (Tok.Kind != ',') break;
    GetNextToken(); // eat the ','.

    if (Tok.Kind != tok::tok_identifier)
      return Error("expected identifier list after var");
  }

  // At this point, we have to have 'in'.
  if (Tok.Kind != tok::tok_in)
    return Error("expected 'in' keyword after 'var'");
  GetNextToken();  // eat 'in'.

  ExprAST *Body = ParseExpression();
  if (Body == 0) return 0;

  return new VarExprAST(VarNames, Body);
}


/// primary
///   ::= identifierexpr
///   ::= numberexpr
///   ::= parenexpr
///   ::= ifexpr
///   ::= forexpr
///   ::= varexpr
ExprAST *Parser::ParsePrimary() {
  switch (Tok.Kind) {
  default: return Error("unknown token when expecting an expression");
  case tok::tok_identifier: return ParseIdentifierExpr();
  case tok::tok_number:     return ParseNumberExpr();
  case '(':				          return ParseParenExpr();
  case tok::tok_if:         return ParseIfExpr();
  case tok::tok_for:        return ParseForExpr();
  case tok::tok_var:        return ParseVarExpr();
  }
}


/// unary
///   ::= primary
///   ::= '!' unary
ExprAST *Parser::ParseUnary() {
  // If the current token is not an operator, it must be a primary expr.
  if (!isascii(Tok.Kind) || Tok.Kind == '(' || Tok.Kind == ',')
    return ParsePrimary();

  // If this is a unary operator, read it.
  int Opc = Tok.Kind;
  GetNextToken();
  if (ExprAST *Operand = ParseUnary())
    return new UnaryExprAST(Opc, Operand);
  return 0;
}


/// binoprhs
///   ::= ('+' unary)*
ExprAST *Parser::ParseBinOpRHS(int ExprPrec, ExprAST *LHS) {
  // If this is a binop, find its precedence.
  while (1) {
    int TokPrec = Tok.GetTokPrecedence();

    // If this is a binop that binds at least as tightly as the current binop,
    // consume it, otherwise we are done.
    if (TokPrec < ExprPrec)
      return LHS;

    // Okay, we know this is a binop.
    int BinOp = Tok.Kind;
    GetNextToken();  // eat binop

    // Parse the unary expression after the binary operator.
    ExprAST *RHS = ParseUnary();
    if (!RHS) return 0;

    // If BinOp binds less tightly with RHS than the operator after RHS, let
    // the pending operator take RHS as its LHS.
    int NextPrec = Tok.GetTokPrecedence();
    if (TokPrec < NextPrec) {
      RHS = ParseBinOpRHS(TokPrec+1, RHS);
      if (RHS == 0) return 0;
    }

    // Merge LHS/RHS.
    LHS = new BinaryExprAST(BinOp, LHS, RHS);
  }
}


/// expression
///   ::= unary binoprhs
///
ExprAST *Parser::ParseExpression() {
  ExprAST *LHS = ParseUnary();
  if (!LHS) return 0;

  return ParseBinOpRHS(0, LHS);
}


/// prototype
///   ::= id '(' id* ')'
///   ::= binary LETTER number? (id, id)
///   ::= unary LETTER (id)
PrototypeAST *Parser::ParsePrototype() {

  std::string FnName;

  unsigned Kind = 0; // 0 = identifier, 1 = unary, 2 = binary.
  unsigned BinaryPrecedence = 30;

  switch (Tok.Kind) {
  default:
    return ErrorP("Expected function name in prototype");
  case tok::tok_identifier:
    FnName = Tok.IdentifierStr;
    Kind = 0;
    GetNextToken();
    break;
  case tok::tok_unary:
    GetNextToken();
    if (!isascii(Tok.Kind))
      return ErrorP("Expected unary operator");
    FnName = "unary";
    FnName += (char)Tok.Kind;
    Kind = 1;
    GetNextToken();
    break;
  case tok::tok_binary:
    GetNextToken();
    if (!isascii(Tok.Kind))
      return ErrorP("Expected binary operator");
    FnName = "binary";
    FnName += (char)Tok.Kind;
    Kind = 2;
    GetNextToken();

    // Read the precedence if present.
    if (Tok.Kind == tok::tok_number) {
      if (Tok.NumVal < 1 || Tok.NumVal > 100)
        return ErrorP("Invalid precedecnce: must be 1..100");
      BinaryPrecedence = (unsigned)(Tok.NumVal);
      GetNextToken();
    }
    break;
  }

  if (Tok.Kind != '(')
    return ErrorP("Expected '(' in prototype");

  std::vector<std::string> ArgNames;
  while (GetNextToken() == tok::tok_identifier)
    ArgNames.push_back(Tok.IdentifierStr);
  if (Tok.Kind != ')')
    return ErrorP("Expected ')' in prototype");

  // success.
  GetNextToken();  // eat ')'.

  // Verify right number of names for operator.
  if (Kind && ArgNames.size() != Kind)
    return ErrorP("Invalid number of operands for operator");

  return new PrototypeAST(FnName, ArgNames, Kind != 0, BinaryPrecedence);
}


/// definition ::= 'def' prototype expression
FunctionAST *Parser::ParseDefinition() {
  GetNextToken();  // eat def.
  PrototypeAST *Proto = ParsePrototype();
  if (Proto == 0) return 0;

  if (ExprAST *E = ParseExpression())
    return new FunctionAST(Proto, E);
  return 0;
}

/// toplevelexpr ::= expression
FunctionAST *Parser::ParseTopLevelExpr() {
  if (ExprAST *E = ParseExpression()) {
    // Make an anonymous proto.
    PrototypeAST *Proto = new PrototypeAST("", std::vector<std::string>());
    return new FunctionAST(Proto, E);
  }
  return 0;
}

/// external ::= 'extern' prototype
PrototypeAST *Parser::ParseExtern() {
  GetNextToken();  // eat extern.
  return ParsePrototype();
}



//===----------------------------------------------------------------------===//
// Top-Level parsing
//===----------------------------------------------------------------------===//

void Parser::HandleDefinition() {
  if (FunctionAST *F = ParseDefinition()) {
    if (llvm::Function *LF = F->Codegen()) {
      fprintf(stderr, "Read function definition:");
      LF->dump();
    }
    //	if (ParseDefinition()) {}
    //		fprintf(stderr, "Parsed a function definition.\n");
  } else {
    // Skip token for error recovery.
    GetNextToken();
  }
}

void Parser::HandleExtern() {
  if (PrototypeAST *P = ParseExtern()) {
    if (llvm::Function *F = P->Codegen()) {
      fprintf(stderr, "Read extern: ");
      F->dump();
    }
    //	if (ParseExtern()) {}
    //		fprintf(stderr, "Parsed an extern\n");
  } else {
    // Skip token for error recovery.
    GetNextToken();
  }
}

void Parser::HandleTopLevelExpression() {
  // Evaluate a top-level expression into an anonymous function.
  if (FunctionAST *F = ParseTopLevelExpr()) {
    if (llvm::Function *LF = F->Codegen()) {
      //fprintf(stderr, "Read top-level expression:");
      //LF->dump();

      //------------------------------------------------
      // JIT the function, returning a function pointer.
      //------------------------------------------------
      void *FPtr = TheExecutionEngine->getPointerToFunction(LF);

      //------------------------------------------------
      // Cast it to the right type (takes no arguments, returns a double) so we
      // can call it as a native function.
      //------------------------------------------------
      double (*FP)() = (double (*)())(intptr_t)FPtr;
      fprintf(stderr, "Evaluated to %f\n", FP());
    }
    //	if (ParseTopLevelExpr()) {}
    //		fprintf(stderr, "Parsed a top-level expr\n");
  } else {
    // Skip token for error recovery.
    GetNextToken();
  }
}


/// top ::= definition | external | expression | ';'
void Parser::Go() {

  // Prime the first token.
  fprintf(stderr, "ready> ");
  GetNextToken();

  while (1) {
    fprintf(stderr, "ready> ");
    switch (Tok.Kind) {
    case tok::tok_eof:
      return;
    case ';':
      GetNextToken();
      break;  // ignore top-level semicolons.
    case tok::tok_def:
      HandleDefinition();
      break;
    case tok::tok_extern:
      HandleExtern();
      break;
    default:
      HandleTopLevelExpression();
      break;
    }
  }
}


