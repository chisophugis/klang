//===--- Driver.cpp - -------------------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements main entry function.
//
//===----------------------------------------------------------------------===//

#include "klang/AST/ASTNodes.h"
#include "klang/Builtin/Tutorial.h"
#include "klang/Lex/Lexer.h"
#include "klang/Parse/Parser.h"

#include "llvm/Module.h"
#include "llvm/PassManager.h"
#include "llvm/Analysis/Passes.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/JIT.h"
#include "llvm/IRBuilder.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/DataLayout.h"
#include "llvm/Transforms/Scalar.h"

#include <cstdio>
#include <map>
#include <iostream>
#include <cstring>

#include <getopt.h>
#include <stdio.h>

using std::cerr;
using std::endl;

//===----------------------------------------------------------------------===//
// Main driver code.
//===----------------------------------------------------------------------===//

namespace klang {

  std::map<char, int> Token::BinopPrecedence;

  llvm::Module *TheModule;
  llvm::IRBuilder<> Builder(llvm::getGlobalContext());
  std::map<std::string, llvm::AllocaInst*> NamedValues;

  llvm::FunctionPassManager *TheFPM;
  llvm::ExecutionEngine *TheExecutionEngine;

	bool bUseFile = false;
}

#define ARRAY_SIZE_OF_FILE_PATH	256
FILE*	fpInStream = 0;

int main(int argc, char* const argv[]) {

	int oc = 0;
	int do_help = 0;
	int do_verbose = 0;
	int do_emit_llvm = 0;
	int do_emit_llvm_only = 0;
	char input_file[ARRAY_SIZE_OF_FILE_PATH] = {0,};
	char output_file[ARRAY_SIZE_OF_FILE_PATH] = {0,};


	struct option longopts[] = {
		{"help",						no_argument,				&do_help,						1},
		{"verbose",					no_argument,				&do_verbose,				1},
		{"emit-llvm",				no_argument,				&do_emit_llvm,			1},
		{"emit-llvm-only",	no_argument,				&do_emit_llvm_only,	1},
		{0,0,0,0},
	};

	while ((oc = getopt_long(argc, argv, ":hvo:", longopts, NULL)) != -1) {
		switch(oc) {
			case 'h':
				do_help = 1;
				break;
			case 'v':
				do_verbose = 1;
				break;
			case 'o':
				if (optarg)
					std::strncpy(output_file, optarg, ARRAY_SIZE_OF_FILE_PATH-1);
				break;
			case 0:
				break;
			case ':':
				// missing option argument
				cerr << "Argument is needed!" << endl;
				break;
			case '?':
			default:
				// invalid option
				cerr << "Invalid argument!" << endl;
				break;
		}
	}


	fpInStream = fdopen(0, "r");

	if (argc > 1 && argc == optind + 1) {
		// Last argument after options specifies the input file name.
		if (!std::strcmp(argv[argc-1], "-")) {
			// Placing '-' instead of file name makes klang take STDIN as it's input file.
			// This is one of POSIX convention.
			klang::bUseFile  = false;
		} else {
			if (argv[argc-1]) {
				std::strncpy(input_file, argv[argc-1], ARRAY_SIZE_OF_FILE_PATH-1);
				fpInStream = freopen(input_file, "r", fpInStream);
				klang::bUseFile  = true;
			}
		}
	}

  llvm::InitializeNativeTarget();
  llvm::LLVMContext &Context = llvm::getGlobalContext();

  klang::Lexer myLexer;
  klang::Parser myParser(myLexer);

  // Install standard binary operators.
  // 1 is lowest precedence.
  klang::Token::BinopPrecedence['='] = 2;
  klang::Token::BinopPrecedence['<'] = 10;
  klang::Token::BinopPrecedence['+'] = 20;
  klang::Token::BinopPrecedence['-'] = 20;
  klang::Token::BinopPrecedence['*'] = 40;  // highest.

  // Make the module, which holds all the code.
  klang::TheModule = new llvm::Module("my cool jit", Context);

  //-----------------------------------------------------
  // Create the JIT.  This takes ownership of the module.
  std::string ErrStr;
  klang::TheExecutionEngine =
    llvm::EngineBuilder(klang::TheModule).setErrorStr(&ErrStr).create();
  if (!klang::TheExecutionEngine) {
		cerr << "Could not create ExecutionEngine: " << ErrStr.c_str() << endl;
    exit(1);
  }

  llvm::FunctionPassManager OurFPM(klang::TheModule);

  // Set up the optimizer pipeline.  Start with registering info about how the
  // target lays out data structures.
	OurFPM.add(new llvm::DataLayout(*klang::TheExecutionEngine->getDataLayout()));
  // Provide basic AliasAnalysis support for GVN.
  OurFPM.add(llvm::createBasicAliasAnalysisPass());
  // Promote allocas to registers.
  OurFPM.add(llvm::createPromoteMemoryToRegisterPass());
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
	if (!klang::bUseFile)
  	klang::TheModule->dump();

	// Calls an unused function just not to lose it in the final binary
	// Without this call klangBuiltin.a is just ignored during linking
	putchard('\n');

	fclose(fpInStream);

  return 0;
}

