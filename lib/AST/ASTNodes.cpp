
#include <map>
#include "llvm/Module.h"
#include "llvm/Constants.h"
#include "llvm/Support/IRBuilder.h"
#include "llvm/Analysis/Verifier.h"
#include "klang/AST/ASTNodes.h"
#include "klang/Driver/Utils.h"
#include "klang/Driver/Driver.h"

//===----------------------------------------------------------------------===//
// Code Generation
//===----------------------------------------------------------------------===//


using namespace klang;

llvm::Value *NumberExprAST::Codegen() {
	return llvm::ConstantFP::get(llvm::getGlobalContext(), llvm::APFloat(Val));
}

llvm::Value *VariableExprAST::Codegen() {
	// Look this variable up in the function.
	llvm::Value *V = NamedValues[Name];
	return V ? V : ErrorV("Unknown variable name");
}

llvm::Value *BinaryExprAST::Codegen() {
	llvm::Value *L = LHS->Codegen();
	llvm::Value *R = RHS->Codegen();
	if (L == 0 || R == 0) return 0;

	switch (Op) {
		case '+': return Builder.CreateFAdd(L, R, "addtmp");
		case '-': return Builder.CreateFSub(L, R, "subtmp");
		case '*': return Builder.CreateFMul(L, R, "multmp");
		case '<':
							L = Builder.CreateFCmpULT(L, R, "cmptmp");
							// Convert bool 0/1 to double 0.0 or 1.0
							return Builder.CreateUIToFP(L, llvm::Type::getDoubleTy(llvm::getGlobalContext()),
									"booltmp");
		default: return ErrorV("invalid binary operator");
	}
}

llvm::Value *CallExprAST::Codegen() {
	// Look up the name in the global module table.
	llvm::Function *CalleeF = TheModule->getFunction(Callee);
	if (CalleeF == 0)
		return ErrorV("Unknown function referenced");

	// If argument mismatch error.
	if (CalleeF->arg_size() != Args.size())
		return ErrorV("Incorrect # arguments passed");

	std::vector<llvm::Value*> ArgsV;
	for (unsigned i = 0, e = Args.size(); i != e; ++i) {
		ArgsV.push_back(Args[i]->Codegen());
		if (ArgsV.back() == 0) return 0;
	}

	return Builder.CreateCall(CalleeF, ArgsV, "calltmp");
}

llvm::Function *PrototypeAST::Codegen() {
	// Make the function type:  double(double,double) etc.
	std::vector<llvm::Type*> Doubles(Args.size(),
			llvm::Type::getDoubleTy(llvm::getGlobalContext()));
	llvm::FunctionType *FT = llvm::FunctionType::get(llvm::Type::getDoubleTy(llvm::getGlobalContext()),
			Doubles, false);

	llvm::Function *F = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, Name, TheModule);

	// If F conflicted, there was already something named 'Name'.  If it has a
	// body, don't allow redefinition or reextern.
	if (F->getName() != Name) {
		// Delete the one we just made and get the existing one.
		F->eraseFromParent();
		F = TheModule->getFunction(Name);

		// If F already has a body, reject this.
		if (!F->empty()) {
			ErrorF("redefinition of function");
			return 0;
		}

		// If F took a different number of args, reject.
		if (F->arg_size() != Args.size()) {
			ErrorF("redefinition of function with different # args");
			return 0;
		}
	}

	// Set names for all arguments.
	unsigned Idx = 0;
	for (llvm::Function::arg_iterator AI = F->arg_begin(); Idx != Args.size();
			++AI, ++Idx) {
		AI->setName(Args[Idx]);

		// Add arguments to variable symbol table.
		NamedValues[Args[Idx]] = AI;
	}

	return F;
}

llvm::Function *FunctionAST::Codegen() {
	NamedValues.clear();

	llvm::Function *TheFunction = Proto->Codegen();
	if (TheFunction == 0)
		return 0;

	// Create a new basic block to start insertion into.
	llvm::BasicBlock *BB = llvm::BasicBlock::Create(llvm::getGlobalContext(), "entry", TheFunction);
	Builder.SetInsertPoint(BB);

	if (llvm::Value *RetVal = Body->Codegen()) {
		// Finish off the function.
		Builder.CreateRet(RetVal);

		// Validate the generated code, checking for consistency.
		llvm::verifyFunction(*TheFunction);

		//----------------------
		// Optimize the function.
		//----------------------
		TheFPM->run(*TheFunction);

		return TheFunction;
	}

	// Error reading body, remove function.
	TheFunction->eraseFromParent();
	return 0;
}


