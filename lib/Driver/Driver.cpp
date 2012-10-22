
#include <map>
#include "llvm/Module.h"
#include "llvm/Support/IRBuilder.h"
#include "klang/Parse/Parser.h"
#include "klang/AST/ASTNodes.h"
#include "klang/Lex/Lexer.h"


//===----------------------------------------------------------------------===//
// Main driver code.
//===----------------------------------------------------------------------===//

namespace klang {

	std::map<char, int> Token::BinopPrecedence;

	llvm::Module *TheModule;
	llvm::IRBuilder<> Builder(llvm::getGlobalContext());
	std::map<std::string, llvm::Value*> NamedValues;

}


int main() {
	llvm::LLVMContext &Context = llvm::getGlobalContext();

	klang::Lexer myLexer;
	klang::Parser myParser(myLexer);

	// Install standard binary operators.
	// 1 is lowest precedence.
	klang::Token::BinopPrecedence['<'] = 10;
	klang::Token::BinopPrecedence['+'] = 20;
	klang::Token::BinopPrecedence['-'] = 20;
	klang::Token::BinopPrecedence['*'] = 40;  // highest.

	// Make the module, which holds all the code.
	klang::TheModule = new llvm::Module("my cool jit", Context);

	// Run the main "interpreter loop" now.
	myParser.Go();

	// Print out all of the generated code.
	klang::TheModule->dump();
	return 0;
}

