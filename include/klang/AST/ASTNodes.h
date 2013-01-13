#ifndef KLANG_ASTNODES_H
#define KLANG_ASTNODES_H

#include "llvm/IR/Module.h"
#include <string>
#include <vector>

namespace klang {

  //===--------------------------------------------------------------------===//
  // Abstract Syntax Tree (aka Parse Tree)
  //===--------------------------------------------------------------------===//

  /// ExprAST - Base class for all expression nodes.
  class ExprAST {
  public:
    virtual ~ExprAST() {}
    virtual llvm::Value *Codegen() = 0;
  };

  /// NumberExprAST - Expression class for numeric literals like "1.0".
  class NumberExprAST : public ExprAST {
    double Val;
  public:
    NumberExprAST(double val) : Val(val) {}
    virtual llvm::Value *Codegen();
  };

  /// VariableExprAST - Expression class for referencing a variable, like "a".
  class VariableExprAST : public ExprAST {
    std::string Name;
  public:
    VariableExprAST(const std::string &name) : Name(name) {}
    virtual llvm::Value *Codegen();
  };

  /// UnaryExprAST - Expression class for a unary operator.
  class UnaryExprAST : public ExprAST {
    char Opcode;
    ExprAST *Operand;
  public:
    UnaryExprAST(char opcode, ExprAST *operand)
      : Opcode(opcode), Operand(operand) {}
    virtual llvm::Value *Codegen();
  };

  /// BinaryExprAST - Expression class for a binary operator.
  class BinaryExprAST : public ExprAST {
    char Op;
    ExprAST *LHS, *RHS;
  public:
    BinaryExprAST(char op, ExprAST *lhs, ExprAST *rhs)
      : Op(op), LHS(lhs), RHS(rhs) {}
    virtual llvm::Value *Codegen();
  };

  /// CallExprAST - Expression class for function calls.
  class CallExprAST : public ExprAST {
    std::string Callee;
    std::vector<ExprAST*> Args;
  public:
    CallExprAST(const std::string &callee, std::vector<ExprAST*> &args)
      : Callee(callee), Args(args) {}
    virtual llvm::Value *Codegen();
  };

  /// IfExprAST - Expression class for if/then/else.
  class IfExprAST : public ExprAST {
    ExprAST *Cond, *Then, *Else;
  public:
    IfExprAST(ExprAST *cond, ExprAST *then, ExprAST *_else)
      : Cond(cond), Then(then), Else(_else) {}
    virtual llvm::Value *Codegen();
  };

  /// ForExprAST - Expression class for for/in.
  class ForExprAST : public ExprAST {
    std::string VarName;
    ExprAST *Start, *End, *Step, *Body;
  public:
    ForExprAST(const std::string &varname, ExprAST *start, ExprAST *end,
               ExprAST *step, ExprAST *body)
      : VarName(varname), Start(start), End(end), Step(step), Body(body) {}
    virtual llvm::Value *Codegen();
  };

  /// PrototypeAST - This class represents the "prototype" for a function,
  /// which captures its name, and its argument names
  /// (thus implicitly the number of arguments the function takes),
  /// as well as if it is an operator.
  class PrototypeAST {
    std::string Name;
    std::vector<std::string> Args;
    bool isOperator;
    unsigned Precedence;  // Precedence if a binary op.
  public:
    PrototypeAST(const std::string &name, const std::vector<std::string> &args,
                 bool isoperator = false, unsigned prec = 0)
      : Name(name), Args(args), isOperator(isoperator), Precedence(prec) {}

    bool isUnaryOp() const { return isOperator && Args.size() == 1; }
    bool isBinaryOp() const { return isOperator && Args.size() == 2; }

    char getOperatorName() const {
      assert(isUnaryOp() || isBinaryOp());
      return Name[Name.size()-1];
    }

    unsigned getBinaryPrecedence() const { return Precedence; }

    llvm::Function *Codegen();

  };

  /// FunctionAST - This class represents a function definition itself.
  class FunctionAST {
    PrototypeAST *Proto;
    ExprAST *Body;
  public:
    FunctionAST(PrototypeAST *proto, ExprAST *body)
      : Proto(proto), Body(body) {}
    llvm::Function *Codegen();

  };

}

#endif //#ifndef KLANG_ASTNODES_H

