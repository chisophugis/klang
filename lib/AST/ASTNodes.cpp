
#include <map>
#include "klang/AST/ASTNodes.h"
#include "klang/Driver/Utils.h"

//===----------------------------------------------------------------------===//
// Code Generation
//===----------------------------------------------------------------------===//

using namespace llvm;

namespace klang {

	extern Module *TheModule;
	extern IRBuilder<> Builder;
	extern std::map<std::string, Value*> NamedValues;

	Value *NumberExprAST::Codegen() {
		return ConstantFP::get(getGlobalContext(), APFloat(Val));
	}

	Value *VariableExprAST::Codegen() {
		// Look this variable up in the function.
		Value *V = NamedValues[Name];
		return V ? V : ErrorV("Unknown variable name");
	}

	Value *BinaryExprAST::Codegen() {
		Value *L = LHS->Codegen();
		Value *R = RHS->Codegen();
		if (L == 0 || R == 0) return 0;

		switch (Op) {
			case '+': return Builder.CreateFAdd(L, R, "addtmp");
			case '-': return Builder.CreateFSub(L, R, "subtmp");
			case '*': return Builder.CreateFMul(L, R, "multmp");
			case '<':
								L = Builder.CreateFCmpULT(L, R, "cmptmp");
								// Convert bool 0/1 to double 0.0 or 1.0
								return Builder.CreateUIToFP(L, Type::getDoubleTy(getGlobalContext()),
										"booltmp");
			default: return ErrorV("invalid binary operator");
		}
	}

	Value *CallExprAST::Codegen() {
		// Look up the name in the global module table.
		Function *CalleeF = TheModule->getFunction(Callee);
		if (CalleeF == 0)
			return ErrorV("Unknown function referenced");

		// If argument mismatch error.
		if (CalleeF->arg_size() != Args.size())
			return ErrorV("Incorrect # arguments passed");

		std::vector<Value*> ArgsV;
		for (unsigned i = 0, e = Args.size(); i != e; ++i) {
			ArgsV.push_back(Args[i]->Codegen());
			if (ArgsV.back() == 0) return 0;
		}

		return Builder.CreateCall(CalleeF, ArgsV, "calltmp");
	}

	Function *PrototypeAST::Codegen() {
		// Make the function type:  double(double,double) etc.
		std::vector<Type*> Doubles(Args.size(),
				Type::getDoubleTy(getGlobalContext()));
		FunctionType *FT = FunctionType::get(Type::getDoubleTy(getGlobalContext()),
				Doubles, false);

		Function *F = Function::Create(FT, Function::ExternalLinkage, Name, TheModule);

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
		for (Function::arg_iterator AI = F->arg_begin(); Idx != Args.size();
				++AI, ++Idx) {
			AI->setName(Args[Idx]);

			// Add arguments to variable symbol table.
			NamedValues[Args[Idx]] = AI;
		}

		return F;
	}

	Function *FunctionAST::Codegen() {
		NamedValues.clear();

		Function *TheFunction = Proto->Codegen();
		if (TheFunction == 0)
			return 0;

		// Create a new basic block to start insertion into.
		BasicBlock *BB = BasicBlock::Create(getGlobalContext(), "entry", TheFunction);
		Builder.SetInsertPoint(BB);

		if (Value *RetVal = Body->Codegen()) {
			// Finish off the function.
			Builder.CreateRet(RetVal);

			// Validate the generated code, checking for consistency.
			verifyFunction(*TheFunction);

			return TheFunction;
		}

		// Error reading body, remove function.
		TheFunction->eraseFromParent();
		return 0;
	}

}

