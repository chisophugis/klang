
#include <map>
#include "llvm/Module.h"
#include "llvm/PassManager.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/Support/IRBuilder.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetData.h"
#include "llvm/Analysis/Passes.h"
#include "llvm/Transforms/Scalar.h"
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

	llvm::FunctionPassManager *TheFPM;
	llvm::ExecutionEngine *TheExecutionEngine;

}


int main() {
	llvm::InitializeNativeTarget();
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

	//-----------------------------------------------------
	// Create the JIT.  This takes ownership of the module.
	std::string ErrStr;
	klang::TheExecutionEngine = llvm::EngineBuilder(klang::TheModule).setErrorStr(&ErrStr).create();
	if (!klang::TheExecutionEngine) {
		fprintf(stderr, "Could not create ExecutionEngine: %s\n", ErrStr.c_str());
		exit(1);
	}

	llvm::FunctionPassManager OurFPM(klang::TheModule);

	// Set up the optimizer pipeline.  Start with registering info about how the
	// target lays out data structures.
	OurFPM.add(new llvm::TargetData(*klang::TheExecutionEngine->getTargetData()));
	// Provide basic AliasAnalysis support for GVN.
	OurFPM.add(llvm::createBasicAliasAnalysisPass());
	// Do simple "peephole" optimizations and bit-twiddling optzns.
	OurFPM.add(llvm::createInstructionCombiningPass());
	// Reassociate expressions.
	OurFPM.add(llvm::createReassociatePass());
	// Eliminate Common SubExpressions.
	OurFPM.add(llvm::createGVNPass());
	// Simplify the control flow graph (deleting unreachable blocks, etc).
	OurFPM.add(llvm::createCFGSimplificationPass());

	OurFPM.doInitialization();

	// Set the global so the code gen can use this.
	klang::TheFPM = &OurFPM;
	//-----------------------------------------------------

	// Run the main "interpreter loop" now.
	myParser.Go();

	klang::TheFPM = 0;

	// Print out all of the generated code.
	klang::TheModule->dump();
	return 0;
}

