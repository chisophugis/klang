#ifndef KLANG_LEXER_H
#define KLANG_LEXER_H

#include "Token.h"

namespace klang {

	class Lexer {

		int LastChar;

		/// gettok - Return the next token from standard input.
		void gettok(Token &Result);

		public:
		Lexer();

		void Lex(Token &Result);

	};

}

#endif //#ifndef KLANG_LEXER_H
