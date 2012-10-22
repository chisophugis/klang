#include <cstdio>
#include "llvm/Module.h"
#include "klang/Driver/Utils.h"

using namespace klang;

//Error* - These are little helper functions for error handling.
ExprAST *klang::Error(const char *Str) { 
	fprintf(stderr, "Error: %s\n", Str);
	return 0;
}

PrototypeAST *klang::ErrorP(const char *Str) { 
	klang::Error(Str); 
	return 0; 
}

FunctionAST *klang::ErrorF(const char *Str) { 
	klang::Error(Str); 
	return 0; 
}

llvm::Value *klang::ErrorV(const char *Str) { 
	klang::Error(Str); 
	return 0; 
}

