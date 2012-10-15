#ifndef KLANG_PARSER_H
#define KLANG_PARSER_H

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

		void GetNextToken() {
			Lxr.Lex(Tok);
		}

		ExprAST *ParseIdentifierExpr();
		ExprAST *ParseNumberExpr();
		ExprAST *ParseParenExpr();
		ExprAST *ParsePrimary();
		ExprAST *ParseBinOpRHS(int ExprPrec, ExprAST *LHS);
		ExprAST *ParseExpression();
		PrototypeAST *ParsePrototype();
		FunctionAST *ParseDefinition();
		FunctionAST *ParseTopLevelExpr();
		PrototypeAST *ParseExtern();

	};

}

#endif //#ifndef KLANG_PARSER_H

