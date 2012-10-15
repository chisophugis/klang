
#include "Parser.h"


/// BinopPrecedence - This holds the precedence for each binary operator that is
/// defined.                  
static std::map<char, int> BinopPrecedence;

/// GetTokPrecedence - Get the precedence of the pending binary operator token.
static int GetTokPrecedence() {
	if (!isascii(CurTok))
		return -1;

	// Make sure it's a declared binop.
	int TokPrec = BinopPrecedence[CurTok];
	if (TokPrec <= 0) return -1;
	return TokPrec;
}

/// Error* - These are little helper functions for error handling.
ExprAST *Error(const char *Str) { fprintf(stderr, "Error: %s\n", Str);return 0;}
PrototypeAST *ErrorP(const char *Str) { Error(Str); return 0; }
FunctionAST *ErrorF(const char *Str) { Error(Str); return 0; }

/// identifierexpr
///   ::= identifier
///   ::= identifier '(' expression* ')'
static ExprAST *Parser::ParseIdentifierExpr() {
	std::string IdName = IdentifierStr;

	getNextToken();  // eat identifier.

	if (CurTok != '(') // Simple variable ref.
		return new VariableExprAST(IdName);

	// Call.
	getNextToken();  // eat (
	std::vector<ExprAST*> Args;
	if (CurTok != ')') {
		while (1) {
			ExprAST *Arg = ParseExpression();
			if (!Arg) return 0;
			Args.push_back(Arg);

			if (CurTok == ')') break;

			if (CurTok != ',')
				return Error("Expected ')' or ',' in argument list");
			getNextToken();
		}
	}

	// Eat the ')'.
	getNextToken();

	return new CallExprAST(IdName, Args);
}



/// numberexpr ::= number
static ExprAST *Parser::ParseNumberExpr() {
	ExprAST *Result = new NumberExprAST(NumVal);
	getNextToken(); // consume the number
	return Result;
}

/// parenexpr ::= '(' expression ')'
static ExprAST *Parser::ParseParenExpr() {
	getNextToken();  // eat (.
	ExprAST *V = ParseExpression();
	if (!V) return 0;

	if (CurTok != ')')
		return Error("expected ')'");
	getNextToken();  // eat ).
	return V;
}

/// primary
///   ::= identifierexpr
///   ::= numberexpr
///   ::= parenexpr
static ExprAST *Parser::ParsePrimary() {
	switch (CurTok) {
		default: return Error("unknown token when expecting an expression");
		case tok_identifier: return ParseIdentifierExpr();
		case tok_number:     return ParseNumberExpr();
		case '(':            return ParseParenExpr();
	}
}



/// binoprhs
///   ::= ('+' primary)*
static ExprAST *Parser::ParseBinOpRHS(int ExprPrec, ExprAST *LHS) {
	// If this is a binop, find its precedence.
	while (1) {
		int TokPrec = GetTokPrecedence();

		// If this is a binop that binds at least as tightly as the current binop,
		// consume it, otherwise we are done.
		if (TokPrec < ExprPrec)
			return LHS;

		// Okay, we know this is a binop.
		int BinOp = CurTok;
		getNextToken();  // eat binop

		// Parse the primary expression after the binary operator.
		ExprAST *RHS = ParsePrimary();
		if (!RHS) return 0;

		// If BinOp binds less tightly with RHS than the operator after RHS, let
		// the pending operator take RHS as its LHS.
		int NextPrec = GetTokPrecedence();
		if (TokPrec < NextPrec) {
			RHS = ParseBinOpRHS(TokPrec+1, RHS);
			if (RHS == 0) return 0;
		}

		// Merge LHS/RHS.
		LHS = new BinaryExprAST(BinOp, LHS, RHS);
	}
}




/// expression
///   ::= primary binoprhs
///
static ExprAST *Parser::ParseExpression() {
	ExprAST *LHS = ParsePrimary();
	if (!LHS) return 0;

	return ParseBinOpRHS(0, LHS);
}

/// prototype
///   ::= id '(' id* ')'
static PrototypeAST *Parser::ParsePrototype() {
	if (CurTok != tok_identifier)
		return ErrorP("Expected function name in prototype");

	std::string FnName = IdentifierStr;
	getNextToken();

	if (CurTok != '(')
		return ErrorP("Expected '(' in prototype");

	std::vector<std::string> ArgNames;
	while (getNextToken() == tok_identifier)
		ArgNames.push_back(IdentifierStr);
	if (CurTok != ')')
		return ErrorP("Expected ')' in prototype");

	// success.
	getNextToken();  // eat ')'.

	return new PrototypeAST(FnName, ArgNames);
}




/// definition ::= 'def' prototype expression
static FunctionAST *Parser::ParseDefinition() {
	getNextToken();  // eat def.
	PrototypeAST *Proto = ParsePrototype();
	if (Proto == 0) return 0;

	if (ExprAST *E = ParseExpression())
		return new FunctionAST(Proto, E);
	return 0;
}

/// toplevelexpr ::= expression
static FunctionAST *Parser::ParseTopLevelExpr() {
	if (ExprAST *E = ParseExpression()) {
		// Make an anonymous proto.
		PrototypeAST *Proto = new PrototypeAST("", std::vector<std::string>());
		return new FunctionAST(Proto, E);
	}
	return 0;
}

/// external ::= 'extern' prototype
static PrototypeAST *Parser::ParseExtern() {
	getNextToken();  // eat extern.
	return ParsePrototype();
}

