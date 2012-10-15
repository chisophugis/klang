#include <stdio.h>
#include <ctype.h>
#include "Lexer.h"

using namespace klang;

/// Return the next token from standard input.
void
Lexer::Lex(Token &Result) {

	// Skip any whitespace.
	while (isspace(LastChar))
		LastChar = getchar();

	if (isalpha(LastChar)) { // identifier: [a-zA-Z][a-zA-Z0-9]*
		Result.IdentifierStr = LastChar;
		while (isalnum((LastChar = getchar())))
			Result.IdentifierStr += LastChar;

		if (Result.IdentifierStr == "def") {
			Result.CurTok = tok::tok_def;
			return;
		}
		if (Result.IdentifierStr == "extern") {
			Result.CurTok = tok::tok_extern;
			return;
		}

		Result.CurTok = tok::tok_identifier;
		return;
	}

	if (isdigit(LastChar) || LastChar == '.') {   // Number: [0-9.]+
		std::string NumStr;
		do {
			NumStr += LastChar;
			LastChar = getchar();
		} while (isdigit(LastChar) || LastChar == '.');

		Result.NumVal = strtod(NumStr.c_str(), 0);

		Result.CurTok  = tok::tok_number;
		return;
	}

	if (LastChar == '#') {
		// Comment until end of line.
		do LastChar = getchar();
		while (LastChar != EOF && LastChar != '\n' && LastChar != '\r');

		if (LastChar != EOF) {
			Lex(Result);
			return;
		}
	}

	// Check for end of file.  Don't eat the EOF.
	if (LastChar == EOF) {
		Result.CurTok = tok::tok_eof;
		return;
	}

	// Otherwise, just return the character as its ascii value.
	int ThisChar = LastChar;
	LastChar = getchar();
	Result.CurTok = ThisChar;
	return;
}


Lexer::Lexer() 
	: LastChar(' ')
{
};


