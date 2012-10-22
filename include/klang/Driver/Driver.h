
#ifndef KLANG_DRIVER_H
#define KLANG_DRIVER_H

#include <map>
#include "llvm/Module.h"
#include "llvm/Support/IRBuilder.h"
#include "klang/Parse/Parser.h"
#include "klang/AST/ASTNodes.h"
#include "klang/Lex/Lexer.h"


namespace klang {

	//extern std::map<char, int> Token::BinopPrecedence;

	extern llvm::Module *TheModule;
	extern llvm::IRBuilder<> Builder;
	extern std::map<std::string, llvm::Value*> NamedValues;

}

#endif//#ifndef KLANG_DRIVER_H

