#ifndef KLANG_TOKEN_H
#define KLANG_TOKEN_H


#include <string>
#include "TokenKinds.h"


namespace klang {

	class Token {
		friend class Lexer;

		int CurTok;//Kind

		std::string IdentifierStr;  // Filled in if tok_identifier
		double NumVal;              // Filled in if tok_number

	};

}

#endif //#ifndef KLANG_TOKEN_H
