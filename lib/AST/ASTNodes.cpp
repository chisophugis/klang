
#include "klang/AST/ASTNodes.h"
#include "klang/Driver/Driver.h"
#include "klang/Driver/Utils.h"
#include "llvm/Constant.h"
#include "llvm/Constants.h"
#include "llvm/Instructions.h"
#include "llvm/Module.h"
#include "llvm/Type.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/Support/IRBuilder.h"
#include <map>

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

llvm::Value *UnaryExprAST::Codegen() {
  llvm::Value *OperandV = Operand->Codegen();
  if (OperandV == 0) return 0;

  llvm::Function *F = TheModule->getFunction(std::string("unary")+Opcode);
  if (F == 0)
    return ErrorV("Unknown unary operator");

  return Builder.CreateCall(F, OperandV, "unop");
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
            return Builder.CreateUIToFP(
              L,
              llvm::Type::getDoubleTy(llvm::getGlobalContext()),
              "booltmp");
  default: break;
  }

  // If it wasn't a builtin binary operator, it must be a user defined one. Emit
  // a call to it.
  llvm::Function *F = TheModule->getFunction(std::string("binary")+Op);
  assert(F && "binary operator not found!");

  llvm::Value *Ops[2] = { L, R };
  return Builder.CreateCall(F, Ops, "binop");
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

llvm::Value *IfExprAST::Codegen() {
  llvm::Value *CondV = Cond->Codegen();
  if (CondV == 0) return 0;

  // Convert condition to a bool by comparing equal to 0.0.
  CondV = Builder.CreateFCmpONE(
    CondV,
    llvm::ConstantFP::get(llvm::getGlobalContext(), llvm::APFloat(0.0)),
    "ifcond");

  llvm::Function *TheFunction = Builder.GetInsertBlock()->getParent();

  // Create blocks for the then and else cases.  Insert the 'then' block at the
  // end of the function.
  llvm::BasicBlock *ThenBB = llvm::BasicBlock::Create(
    llvm::getGlobalContext(),
    "then",
    TheFunction);
  llvm::BasicBlock *ElseBB = llvm::BasicBlock::Create(
    llvm::getGlobalContext(),
    "else");
  llvm::BasicBlock *MergeBB = llvm::BasicBlock::Create(
    llvm::getGlobalContext(),
    "ifcont");

  Builder.CreateCondBr(CondV, ThenBB, ElseBB);

  // Emit then value.
  Builder.SetInsertPoint(ThenBB);

  llvm::Value *ThenV = Then->Codegen();
  if (ThenV == 0) return 0;

  Builder.CreateBr(MergeBB);
  // Codegen of 'Then' can change the current block, update ThenBB for the PHI.
  ThenBB = Builder.GetInsertBlock();

  // Emit else block.
  TheFunction->getBasicBlockList().push_back(ElseBB);
  Builder.SetInsertPoint(ElseBB);

  llvm::Value *ElseV = Else->Codegen();
  if (ElseV == 0) return 0;

  Builder.CreateBr(MergeBB);
  // Codegen of 'Else' can change the current block, update ElseBB for the PHI.
  ElseBB = Builder.GetInsertBlock();

  // Emit merge block.
  TheFunction->getBasicBlockList().push_back(MergeBB);
  Builder.SetInsertPoint(MergeBB);
  llvm::PHINode *PN = Builder.CreatePHI(
    llvm::Type::getDoubleTy(llvm::getGlobalContext()),
    2,
    "iftmp");

  PN->addIncoming(ThenV, ThenBB);
  PN->addIncoming(ElseV, ElseBB);
  return PN;
}

llvm::Value *ForExprAST::Codegen() {
  // Output this as:
  //   ...
  //   start = startexpr
  //   goto loop
  // loop:
  //   variable = phi [start, loopheader], [nextvariable, loopend]
  //   ...
  //   bodyexpr
  //   ...
  // loopend:
  //   step = stepexpr
  //   nextvariable = variable + step
  //   endcond = endexpr
  //   br endcond, loop, endloop
  // outloop:

  // Emit the start code first, without 'variable' in scope.
  llvm::Value *StartVal = Start->Codegen();
  if (StartVal == 0) return 0;

  // Make the new basic block for the loop header, inserting after current
  // block.
  llvm::Function *TheFunction = Builder.GetInsertBlock()->getParent();
  llvm::BasicBlock *PreheaderBB = Builder.GetInsertBlock();
  llvm::BasicBlock *LoopBB = llvm::BasicBlock::Create(
    llvm::getGlobalContext(),
    "loop",
    TheFunction);

  // Insert an explicit fall through from the current block to the LoopBB.
  Builder.CreateBr(LoopBB);

  // Start insertion in LoopBB.
  Builder.SetInsertPoint(LoopBB);

  // Start the PHI node with an entry for Start.
  llvm::PHINode *Variable = Builder.CreatePHI(
    llvm::Type::getDoubleTy(llvm::getGlobalContext()),
    2,
    VarName.c_str());
  Variable->addIncoming(StartVal, PreheaderBB);

  // Within the loop, the variable is defined equal to the PHI node.  If it
  // shadows an existing variable, we have to restore it, so save it now.
  llvm::Value *OldVal = NamedValues[VarName];
  NamedValues[VarName] = Variable;

  // Emit the body of the loop.  This, like any other expr, can change the
  // current BB.  Note that we ignore the value computed by the body, but don't
  // allow an error.
  if (Body->Codegen() == 0)
    return 0;

  // Emit the step value.
  llvm::Value *StepVal;
  if (Step) {
    StepVal = Step->Codegen();
    if (StepVal == 0) return 0;
  } else {
    // If not specified, use 1.0.
    StepVal = llvm::ConstantFP::get(
      llvm::getGlobalContext(),
      llvm::APFloat(1.0));
  }

  llvm::Value *NextVar = Builder.CreateFAdd(Variable, StepVal, "nextvar");

  // Compute the end condition.
  llvm::Value *EndCond = End->Codegen();
  if (EndCond == 0) return EndCond;

  // Convert condition to a bool by comparing equal to 0.0.
  EndCond = Builder.CreateFCmpONE(
    EndCond,
    llvm::ConstantFP::get(llvm::getGlobalContext(), llvm::APFloat(0.0)),
    "loopcond");

  // Create the "after loop" block and insert it.
  llvm::BasicBlock *LoopEndBB = Builder.GetInsertBlock();
  llvm::BasicBlock *AfterBB = llvm::BasicBlock::Create(
    llvm::getGlobalContext(),
    "afterloop",
    TheFunction);

  // Insert the conditional branch into the end of LoopEndBB.
  Builder.CreateCondBr(EndCond, LoopBB, AfterBB);

  // Any new code will be inserted in AfterBB.
  Builder.SetInsertPoint(AfterBB);

  // Add a new entry to the PHI node for the backedge.
  Variable->addIncoming(NextVar, LoopEndBB);

  // Restore the unshadowed variable.
  if (OldVal)
    NamedValues[VarName] = OldVal;
  else
    NamedValues.erase(VarName);

  // for expr always returns 0.0.
  return llvm::Constant::getNullValue(
    llvm::Type::getDoubleTy(llvm::getGlobalContext()));
}


llvm::Function *PrototypeAST::Codegen() {
  // Make the function type:  double(double,double) etc.
  std::vector<llvm::Type*> Doubles(
    Args.size(),
    llvm::Type::getDoubleTy(llvm::getGlobalContext()));
  llvm::FunctionType *FT = llvm::FunctionType::get(
    llvm::Type::getDoubleTy(llvm::getGlobalContext()),
    Doubles,
    false);

  llvm::Function *F = llvm::Function::Create(
    FT,
    llvm::Function::ExternalLinkage,
    Name,
    TheModule);

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

  // If this is an operator, install it.
  if (Proto->isBinaryOp())
    Token::BinopPrecedence[Proto->getOperatorName()] =
      Proto->getBinaryPrecedence();

  // Create a new basic block to start insertion into.
  llvm::BasicBlock *BB = llvm::BasicBlock::Create(
    llvm::getGlobalContext(),
    "entry",
    TheFunction);
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

  if (Proto->isBinaryOp())
    Token::BinopPrecedence.erase(Proto->getOperatorName());
  return 0;
}


