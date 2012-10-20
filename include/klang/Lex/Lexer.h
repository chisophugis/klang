#ifndef KLANG_LEXER_H
#define KLANG_LEXER_H

#include "klang/Lex/Token.h"

namespace klang {

	class Lexer {

		int LastChar;

		public:
		Lexer();

		/// Return the next token from standard input.
		void Lex(Token &Result);

	};

}

#endif //#ifndef KLANG_LEXER_H
