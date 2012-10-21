#include "klang/Parse/Parser.h"
#include "klang/AST/ASTNodes.h"
#include "klang/Lex/Lexer.h"


using namespace llvm;

//===----------------------------------------------------------------------===//
// Main driver code.
//===----------------------------------------------------------------------===//

namespace klang {

	std::map<char, int> Token::BinopPrecedence;

	Module *TheModule;
	IRBuilder<> Builder(getGlobalContext());
	std::map<std::string, Value*> NamedValues;

}

using namespace klang;

int main() {
	LLVMContext &Context = getGlobalContext();

	Lexer myLexer;
	Parser myParser(myLexer);

	// Install standard binary operators.
	// 1 is lowest precedence.
	Token::BinopPrecedence['<'] = 10;
	Token::BinopPrecedence['+'] = 20;
	Token::BinopPrecedence['-'] = 20;
	Token::BinopPrecedence['*'] = 40;  // highest.

	// Make the module, which holds all the code.
	TheModule = new Module("my cool jit", Context);

	// Run the main "interpreter loop" now.
	myParser.Go();

	// Print out all of the generated code.
	TheModule->dump();
	return 0;
}

