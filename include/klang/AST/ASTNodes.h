//===--- ASTNodes.h - -------------------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file defines the AST classes.
///
//===----------------------------------------------------------------------===//

#ifndef KLANG_ASTNODES_H
#define KLANG_ASTNODES_H

#include "llvm/IR/Module.h"
#include "llvm/Support/Casting.h"
#include <string>
#include <vector>

namespace klang {

  //===--------------------------------------------------------------------===//
  // Abstract Syntax Tree (aka Parse Tree)
  //===--------------------------------------------------------------------===//

  /// ExprAST - Base class for all expression nodes.
  class ExprAST {
  public:
		/// Discriminator for LLVM-style RTTI (dyn_cast<> et al.)
		enum ExprKind {
			EK_If,
			EK_For,
			EK_Call,
			EK_Number,
			EK_Binary,
			EK_Var,
			EK_Unary,
			EK_Variable
		};

	private:
		const ExprKind Kind;

	public:
		ExprKind getKind() const { return Kind; }

		ExprAST(ExprKind K) : Kind(K) {}
    virtual ~ExprAST() {}
    virtual llvm::Value *Codegen() = 0;
  };

  /// NumberExprAST - Expression class for numeric literals like "1.0".
  class NumberExprAST : public ExprAST {
    double Val;
  public:
    NumberExprAST(double val) : ExprAST(EK_Number), Val(val) {}

		static bool classof(const ExprAST *E) {
			return E->getKind() == EK_Number;
		}

    virtual llvm::Value *Codegen();
  };

  /// VariableExprAST - Expression class for referencing a variable, like "a".
  class VariableExprAST : public ExprAST {
    std::string Name;
  public:
    VariableExprAST(const std::string &name) : ExprAST(EK_Variable), Name(name) {}

		static bool classof(const ExprAST *E) {
			return E->getKind() == EK_Variable;
		}

    const std::string &getName() const { return Name; }
    virtual llvm::Value *Codegen();
  };

  /// UnaryExprAST - Expression class for a unary operator.
  class UnaryExprAST : public ExprAST {
    char Opcode;
    ExprAST *Operand;
  public:
    UnaryExprAST(char opcode, ExprAST *operand)
      : ExprAST(EK_Unary), Opcode(opcode), Operand(operand) {}

		static bool classof(const ExprAST *E) {
			return E->getKind() == EK_Unary;
		}

    virtual llvm::Value *Codegen();
  };

  /// BinaryExprAST - Expression class for a binary operator.
  class BinaryExprAST : public ExprAST {
    char Op;
    ExprAST *LHS, *RHS;
  public:
    BinaryExprAST(char op, ExprAST *lhs, ExprAST *rhs)
      : ExprAST(EK_Binary), Op(op), LHS(lhs), RHS(rhs) {}

		static bool classof(const ExprAST *E) {
			return E->getKind() == EK_Binary;
		}

    virtual llvm::Value *Codegen();
  };

  /// CallExprAST - Expression class for function calls.
  class CallExprAST : public ExprAST {
    std::string Callee;
    std::vector<ExprAST*> Args;
  public:
    CallExprAST(const std::string &callee, std::vector<ExprAST*> &args)
      : ExprAST(EK_Call), Callee(callee), Args(args) {}

		static bool classof(const ExprAST *E) {
			return E->getKind() == EK_Call;
		}

    virtual llvm::Value *Codegen();
  };

  /// IfExprAST - Expression class for if/then/else.
  class IfExprAST : public ExprAST {
    ExprAST *Cond, *Then, *Else;
  public:
    IfExprAST(ExprAST *cond, ExprAST *then, ExprAST *_else)
      : ExprAST(EK_If), Cond(cond), Then(then), Else(_else) {}

		static bool classof(const ExprAST *E) {
			return E->getKind() == EK_If;
		}

    virtual llvm::Value *Codegen();
  };

  /// ForExprAST - Expression class for for/in.
  class ForExprAST : public ExprAST {
    std::string VarName;
    ExprAST *Start, *End, *Step, *Body;
  public:
    ForExprAST(const std::string &varname, ExprAST *start, ExprAST *end,
               ExprAST *step, ExprAST *body)
      : ExprAST(EK_For), VarName(varname), Start(start), End(end), Step(step), Body(body) {}

		static bool classof(const ExprAST *E) {
			return E->getKind() == EK_For;
		}

    virtual llvm::Value *Codegen();
  };


  /// VarExprAST - Expression class for var/in
  class VarExprAST : public ExprAST {
    std::vector<std::pair<std::string, ExprAST*> > VarNames;
    ExprAST *Body;
  public:
    VarExprAST(const std::vector<std::pair<std::string, ExprAST*> > &varnames,
               ExprAST *body)
      : ExprAST(EK_Var), VarNames(varnames), Body(body) {}

		static bool classof(const ExprAST *E) {
			return E->getKind() == EK_Var;
		}

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

    void CreateArgumentAllocas(llvm::Function *F);
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

