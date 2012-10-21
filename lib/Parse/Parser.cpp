
#include <cstdio>
#include "klang/Parse/Parser.h"
#include "klang/Driver/Utils.h"

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

/// primary
///   ::= identifierexpr
///   ::= numberexpr
///   ::= parenexpr
ExprAST *Parser::ParsePrimary() {
	switch (Tok.Kind) {
		default: return Error("unknown token when expecting an expression");
		case tok::tok_identifier: return ParseIdentifierExpr();
		case tok::tok_number:     return ParseNumberExpr();
		case '(':            return ParseParenExpr();
	}
}



/// binoprhs
///   ::= ('+' primary)*
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

		// Parse the primary expression after the binary operator.
		ExprAST *RHS = ParsePrimary();
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
///   ::= primary binoprhs
///
ExprAST *Parser::ParseExpression() {
	ExprAST *LHS = ParsePrimary();
	if (!LHS) return 0;

	return ParseBinOpRHS(0, LHS);
}

/// prototype
///   ::= id '(' id* ')'
PrototypeAST *Parser::ParsePrototype() {
	if (Tok.Kind != tok::tok_identifier)
		return ErrorP("Expected function name in prototype");

	std::string FnName = Tok.IdentifierStr;
	GetNextToken();

	if (Tok.Kind != '(')
		return ErrorP("Expected '(' in prototype");

	std::vector<std::string> ArgNames;
	while (GetNextToken() == tok::tok_identifier)
		ArgNames.push_back(Tok.IdentifierStr);
	if (Tok.Kind != ')')
		return ErrorP("Expected ')' in prototype");

	// success.
	GetNextToken();  // eat ')'.

	return new PrototypeAST(FnName, ArgNames);
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
		if (Function *LF = F->Codegen()) {
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
		if (Function *F = P->Codegen()) {
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
		if (Function *LF = F->Codegen()) {
			fprintf(stderr, "Read top-level expression:");
			LF->dump();
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


