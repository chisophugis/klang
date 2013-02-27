//===--- Driver.h - ---------------------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file defines global identifiers used globally.
///
//===----------------------------------------------------------------------===//

#ifndef KLANG_DRIVER_H
#define KLANG_DRIVER_H

#include "klang/AST/ASTNodes.h"
#include "klang/Lex/Lexer.h"
#include "klang/Parse/Parser.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/PassManager.h"
#include <map>


namespace klang {

  extern llvm::Module *TheModule;
  extern llvm::IRBuilder<> Builder;
  extern std::map<std::string, llvm::AllocaInst*> NamedValues;

  extern llvm::FunctionPassManager *TheFPM;
  extern llvm::ExecutionEngine *TheExecutionEngine;
}


#endif//#ifndef KLANG_DRIVER_H

