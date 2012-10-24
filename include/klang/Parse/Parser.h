#ifndef KLANG_PARSER_H
#define KLANG_PARSER_H

#include "klang/Lex/Lexer.h"
#include "klang/AST/ASTNodes.h"

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

		ExprAST *ParsePrimary();
		ExprAST *ParseBinOpRHS(int ExprPrec, ExprAST *LHS);
		ExprAST *ParseExpression();
		PrototypeAST *ParsePrototype();
		FunctionAST *ParseDefinition();
		FunctionAST *ParseTopLevelExpr();
		PrototypeAST *ParseExtern();

		//===----------------------------------------------------------------------===//
		// Top-Level parsing
		//===----------------------------------------------------------------------===//

		void HandleDefinition();
		void HandleExtern();
		void HandleTopLevelExpression();
		/// top ::= definition | external | expression | ';'
		void Go();

	};

}

#endif //#ifndef KLANG_PARSER_H

