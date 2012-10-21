#ifndef KLANG_UTIL_H
#define KLANG_UTIL_H

#include "klang/AST/ASTNodes.h"
#include "llvm/DerivedTypes.h"
#include "llvm/LLVMContext.h"
#include "llvm/Module.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/Support/IRBuilder.h"

using namespace llvm;

namespace klang {

	//Error* - These are little helper functions for error handling.
	ExprAST *Error(const char *Str);
	PrototypeAST *ErrorP(const char *Str);
	FunctionAST *ErrorF(const char *Str);
	Value *ErrorV(const char *Str);
}
#endif //#ifndef KLANG_UTIL_H

