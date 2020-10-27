#ifndef HOLEYC_AST_HPP
#define HOLEYC_AST_HPP

#include <ostream>
#include <sstream>
#include <string.h>
#include <list>
#include "err.hpp"
#include "tokens.hpp"
#include "types.hpp"
#include "3ac.hpp"

namespace holeyc {

class TypeAnalysis;

class Opd;

class SymbolTable;
class SemSymbol;

class DerefNode;
class RefNode;
class DeclListNode;
class StmtListNode;
class FormalsListNode;
class DeclNode;
class VarDeclNode;
class StmtNode;
class AssignExpNode;
class FormalDeclNode;
class TypeNode;
class StructTypeNode;
class ExpNode;
class LValNode;
class IDNode;

class ASTNode{
public:
	ASTNode(size_t lineIn, size_t colIn)
	: l(lineIn), c(colIn){ }
	virtual void unparse(std::ostream&, int) = 0;
	size_t line() const { return this->l; }
	size_t col() const { return this->c; }
	std::string pos(){
		return "[" + std::to_string(line()) + ","
			+ std::to_string(col()) + "]";
	}
	virtual std::string nodeKind() = 0;
	virtual bool nameAnalysis(SymbolTable *) = 0;
	//Note that there is no ASTNode::typeAnalysis. To allow
	// for different type signatures, type analysis is 
	// implemented as needed in various subclasses
private:
	size_t l;
	size_t c;
};

class ProgramNode : public ASTNode{
public:
	ProgramNode(std::list<DeclNode *> * globalsIn)
	: ASTNode(1,1), myGlobals(globalsIn){}
	virtual std::string nodeKind() override { return "Program"; }
	void unparse(std::ostream&, int) override;
	virtual bool nameAnalysis(SymbolTable *) override;
	virtual void typeAnalysis(TypeAnalysis *);
	IRProgram * to3AC(TypeAnalysis * ta);
	virtual ~ProgramNode(){ }
private:
	std::list<DeclNode *> * myGlobals;
};

class ExpNode : public ASTNode{
public:
	ExpNode(size_t lIn, size_t cIn) : ASTNode(lIn, cIn){ }
	virtual void unparseNested(std::ostream& out);
	virtual void unparse(std::ostream& out, int indent) override = 0;
	virtual bool nameAnalysis(SymbolTable * symTab) override = 0;
	virtual void typeAnalysis(TypeAnalysis *) = 0;
	virtual Opd * flatten(Procedure * proc) = 0;
};

class LValNode : public ExpNode{
public:
	LValNode(size_t lIn, size_t cIn) : ExpNode(lIn, cIn){}
	virtual std::string nodeKind() override { return "LVal"; }
	void unparse(std::ostream& out, int indent) override = 0;
	void unparseNested(std::ostream& out) override;
	void attachSymbol(SemSymbol * symbolIn) { } 
	bool nameAnalysis(SymbolTable * symTab) override { return false; }
	virtual void typeAnalysis(TypeAnalysis *) override {; } 
	virtual Opd * flatten(Procedure * proc) override;
};

class IDNode : public LValNode{
public:
	IDNode(size_t lIn, size_t cIn, std::string nameIn)
	: LValNode(lIn, cIn), name(nameIn){}
	std::string getName(){ return name; }
	virtual std::string nodeKind() override { return "ID"; }
	void unparse(std::ostream& out, int indent) override;
	void attachSymbol(SemSymbol * symbolIn);
	SemSymbol * getSymbol() const { return mySymbol; }
	bool nameAnalysis(SymbolTable * symTab) override;
	virtual void typeAnalysis(TypeAnalysis *) override;
	virtual Opd * flatten(Procedure * proc) override;
private:
	std::string name;
	SemSymbol * mySymbol = nullptr;
};

class RefNode : public LValNode{
public:
	RefNode(size_t l, size_t c, IDNode * id)
	: LValNode(l, c), myID(id){ }
	void unparse(std::ostream& out, int indent) override;
	std::string nodeKind() override { return "Ref"; }

	virtual bool nameAnalysis(SymbolTable *) override;
	virtual void typeAnalysis(TypeAnalysis *) override;
	virtual Opd * flatten(Procedure * prog) override;
private:
	IDNode * myID;
};

class DerefNode : public LValNode{
public:
	DerefNode(size_t l, size_t c, IDNode * id)
	: LValNode(l, c), myID(id){ }
	void unparse(std::ostream& out, int indent) override;
	std::string nodeKind() override { return "Deref"; }
	virtual bool nameAnalysis(SymbolTable *) override;
	virtual void typeAnalysis(TypeAnalysis *) override;
	virtual Opd * flatten(Procedure * prog) override;
private:
	IDNode * myID;
};

class IndexNode : public LValNode{
public:
	IndexNode(size_t l, size_t c, IDNode * id, ExpNode * offset)
	: LValNode(l, c), myBase(id), myOffset(offset){ }
	void unparse(std::ostream& out, int indent) override;
	std::string nodeKind() override { return "Index"; }
	virtual bool nameAnalysis(SymbolTable * symTab) override;
	virtual void typeAnalysis(TypeAnalysis *) override;
	virtual Opd * flatten(Procedure * prog) override{
		throw new ToDoError("Implement");
	}
private:
	IDNode * myBase;
	ExpNode * myOffset;
};


class TypeNode : public ASTNode{
public:
	TypeNode(size_t l, size_t c) : ASTNode(l, c){ }
	void unparse(std::ostream&, int) override = 0;
	virtual std::string nodeKind() override = 0;
	virtual DataType * getType() = 0;
	virtual bool nameAnalysis(SymbolTable *) override;
	virtual void typeAnalysis(TypeAnalysis *) = 0;
};

class CharTypeNode : public TypeNode{
public:
	CharTypeNode(size_t lIn, size_t cIn, bool isPtrIn)
	: TypeNode(lIn, cIn), isPtr(isPtrIn){}
	void unparse(std::ostream& out, int indent) override;
	std::string nodeKind() override { 
		return "char";
	}
	virtual DataType * getType() override;
	virtual void typeAnalysis(TypeAnalysis *) override;
private:
	bool isPtr;
};

class StmtNode : public ASTNode{
public:
	StmtNode(size_t lIn, size_t cIn) : ASTNode(lIn, cIn){ }
	virtual void unparse(std::ostream& out, int indent) override = 0;
	virtual std::string nodeKind() override = 0;
	virtual void typeAnalysis(TypeAnalysis *) = 0;
	virtual void to3AC(Procedure * proc) = 0;
};

class DeclNode : public StmtNode{
public:
	DeclNode(size_t l, size_t c) : StmtNode(l, c){ }
	void unparse(std::ostream& out, int indent) override =0;
	virtual std::string nodeKind() override = 0;
	virtual void typeAnalysis(TypeAnalysis *) override = 0;
	virtual void to3AC(IRProgram * prog) = 0;
	virtual void to3AC(Procedure * proc) override = 0;
};

class VarDeclNode : public DeclNode{
public:
	VarDeclNode(size_t lIn, size_t cIn, TypeNode * typeIn, IDNode * IDIn)
	: DeclNode(lIn, cIn), myType(typeIn), myID(IDIn){ }
	void unparse(std::ostream& out, int indent) override;
	virtual std::string nodeKind() override { return "VarDecl"; }
	IDNode * ID(){ return myID; }
	TypeNode * getTypeNode(){ return myType; }
	bool nameAnalysis(SymbolTable * symTab) override;
	void typeAnalysis(TypeAnalysis * typing) override;
	virtual void to3AC(Procedure * proc) override;
	virtual void to3AC(IRProgram * prog) override;
private:
	TypeNode * myType;
	IDNode * myID;
};

class FormalDeclNode : public VarDeclNode{
public:
	FormalDeclNode(size_t lIn, size_t cIn, TypeNode * type, IDNode * id) 
	: VarDeclNode(lIn, cIn, type, id){ }
	void unparse(std::ostream& out, int indent) override;
	virtual std::string nodeKind() override { return "FormalDecl"; }
	virtual void to3AC(Procedure * proc) override;
	virtual void to3AC(IRProgram * prog) override;
};

class FnDeclNode : public DeclNode{
public:
	FnDeclNode(size_t lIn, size_t cIn, 
	  TypeNode * retTypeIn, IDNode * idIn,
	  std::list<FormalDeclNode *> * formalsIn,
	  std::list<StmtNode *> * bodyIn)
	: DeclNode(lIn, cIn), 
	  myID(idIn), myRetType(retTypeIn),
	  myFormals(formalsIn), myBody(bodyIn){ }
	IDNode * ID() const { return myID; }
	std::list<FormalDeclNode *> * getFormals() const{
		return myFormals;
	}
	void unparse(std::ostream& out, int indent) override;
	virtual std::string nodeKind() override { return "FnDecl"; }
	virtual bool nameAnalysis(SymbolTable * symTab) override;
	virtual void typeAnalysis(TypeAnalysis *) override;
	void to3AC(IRProgram * prog) override;
	void to3AC(Procedure * prog) override;
	virtual TypeNode * getRetTypeNode() { 
		return myRetType;
	}
private:
	IDNode * myID;
	TypeNode * myRetType;
	std::list<FormalDeclNode *> * myFormals;
	std::list<StmtNode *> * myBody;
};

class AssignStmtNode : public StmtNode{
public:
	AssignStmtNode(size_t l, size_t c, AssignExpNode * expIn)
	: StmtNode(l, c), myExp(expIn){ }
	void unparse(std::ostream& out, int indent) override;
	virtual std::string nodeKind() override { return "AssignStmt"; }
	bool nameAnalysis(SymbolTable * symTab) override;
	virtual void typeAnalysis(TypeAnalysis *) override;
	virtual void to3AC(Procedure * prog) override;
private:
	AssignExpNode * myExp;
};

class FromConsoleStmtNode : public StmtNode{
public:
	FromConsoleStmtNode(size_t l, size_t c, LValNode * dstIn)
	: StmtNode(l, c), myDst(dstIn){ }
	void unparse(std::ostream& out, int indent) override;
	virtual std::string nodeKind() override { return "FromConsoleStmt"; }
	bool nameAnalysis(SymbolTable * symTab) override;
	virtual void typeAnalysis(TypeAnalysis *) override;
	virtual void to3AC(Procedure * prog) override;
private:
	LValNode * myDst;
};

class ToConsoleStmtNode : public StmtNode{
public:
	ToConsoleStmtNode(size_t l, size_t c, ExpNode * srcIn)
	: StmtNode(l, c), mySrc(srcIn){ }
	void unparse(std::ostream& out, int indent) override;
	virtual std::string nodeKind() override { return "ToConsoleStmt"; }
	bool nameAnalysis(SymbolTable * symTab) override;
	virtual void typeAnalysis(TypeAnalysis *) override;
	virtual void to3AC(Procedure * prog) override;
private:
	ExpNode * mySrc;
};

class PostDecStmtNode : public StmtNode{
public:
	PostDecStmtNode(size_t l, size_t c, LValNode * lvalIn)
	: StmtNode(l, c), myLVal(lvalIn){ }
	void unparse(std::ostream& out, int indent) override;
	virtual std::string nodeKind() override { return "PostDecStmt"; }
	virtual bool nameAnalysis(SymbolTable * symTab) override;
	virtual void typeAnalysis(TypeAnalysis *) override;
	virtual void to3AC(Procedure * prog) override;
private:
	LValNode * myLVal;
};

class PostIncStmtNode : public StmtNode{
public:
	PostIncStmtNode(size_t l, size_t c, LValNode * lvalIn)
	: StmtNode(l, c), myLVal(lvalIn){ }
	void unparse(std::ostream& out, int indent) override;
	virtual std::string nodeKind() override { return "PostIncStmt"; }
	virtual bool nameAnalysis(SymbolTable * symTab) override;
	virtual void typeAnalysis(TypeAnalysis *) override;
	virtual void to3AC(Procedure * prog) override;
private:
	LValNode * myLVal;
};

class IfStmtNode : public StmtNode{
public:
	IfStmtNode(size_t l, size_t c, ExpNode * condIn,
	  std::list<StmtNode *> * bodyIn)
	: StmtNode(l, c), myCond(condIn), myBody(bodyIn){ }
	void unparse(std::ostream& out, int indent) override;
	std::string nodeKind() override { return "IfStmt"; }
	bool nameAnalysis(SymbolTable * symTab) override;
	virtual void typeAnalysis(TypeAnalysis *) override;
	virtual void to3AC(Procedure * prog) override;
private:
	ExpNode * myCond;
	std::list<StmtNode *> * myBody;
};

class IfElseStmtNode : public StmtNode{
public:
	IfElseStmtNode(size_t l, size_t c, ExpNode * condIn, 
	  std::list<StmtNode *> * bodyTrueIn,
	  std::list<StmtNode *> * bodyFalseIn)
	: StmtNode(l, c), myCond(condIn),
	  myBodyTrue(bodyTrueIn), myBodyFalse(bodyFalseIn) { }
	void unparse(std::ostream& out, int indent) override;
	std::string nodeKind() override { return "IfElseStmt"; }
	bool nameAnalysis(SymbolTable * symTab) override;
	virtual void typeAnalysis(TypeAnalysis *) override;
	virtual void to3AC(Procedure * prog) override;
private:
	ExpNode * myCond;
	std::list<StmtNode *> * myBodyTrue;
	std::list<StmtNode *> * myBodyFalse;
};

class WhileStmtNode : public StmtNode{
public:
	WhileStmtNode(size_t l, size_t c, ExpNode * condIn, 
	  std::list<StmtNode *> * bodyIn)
	: StmtNode(l, c), myCond(condIn), myBody(bodyIn){ }
	void unparse(std::ostream& out, int indent) override;
	virtual std::string nodeKind() override { return "WhileStmt"; }
	bool nameAnalysis(SymbolTable * symTab) override;
	virtual void typeAnalysis(TypeAnalysis *) override;
	virtual void to3AC(Procedure * prog) override;
private:
	ExpNode * myCond;
	std::list<StmtNode *> * myBody;
};

class ReturnStmtNode : public StmtNode{
public:
	ReturnStmtNode(size_t l, size_t c, ExpNode * exp)
	: StmtNode(l, c), myExp(exp){ }
	void unparse(std::ostream& out, int indent) override;
	virtual std::string nodeKind() override { return "ReturnStmt"; }
	bool nameAnalysis(SymbolTable * symTab) override;
	virtual void typeAnalysis(TypeAnalysis *) override;
	virtual void to3AC(Procedure * proc) override;
private:
	ExpNode * myExp;
};

class CallExpNode : public ExpNode{
public:
	CallExpNode(size_t l, size_t c, IDNode * id,
	  std::list<ExpNode *> * argsIn)
	: ExpNode(l, c), myID(id), myArgs(argsIn){ }
	void unparse(std::ostream& out, int indent) override;
	virtual std::string nodeKind() override { return "CallExp"; }
	bool nameAnalysis(SymbolTable * symTab) override;
	void typeAnalysis(TypeAnalysis *) override;
	DataType * getRetType();

	virtual Opd * flatten(Procedure * proc) override;
private:
	IDNode * myID;
	std::list<ExpNode *> * myArgs;
};

class BinaryExpNode : public ExpNode{
public:
	BinaryExpNode(size_t lIn, size_t cIn, ExpNode * lhs, ExpNode * rhs)
	: ExpNode(lIn, cIn), myExp1(lhs), myExp2(rhs) { }
	bool nameAnalysis(SymbolTable * symTab) override;
	virtual void typeAnalysis(TypeAnalysis *) override = 0;
	virtual Opd * flatten(Procedure * prog) override = 0;
protected:
	ExpNode * myExp1;
	ExpNode * myExp2;
	void binaryLogicTyping(TypeAnalysis * typing);
	void binaryEqTyping(TypeAnalysis * typing);
	void binaryRelTyping(TypeAnalysis * typing);
	void binaryMathTyping(TypeAnalysis * typing);
};

class PlusNode : public BinaryExpNode{
public:
	PlusNode(size_t l, size_t c, ExpNode * e1, ExpNode * e2)
	: BinaryExpNode(l, c, e1, e2){ }
	void unparse(std::ostream& out, int indent) override;
	std::string nodeKind() override { return "Plus"; }
	virtual void typeAnalysis(TypeAnalysis *) override;
	virtual Opd * flatten(Procedure * prog) override;
};

class MinusNode : public BinaryExpNode{
public:
	MinusNode(size_t l, size_t c, ExpNode * e1, ExpNode * e2)
	: BinaryExpNode(l, c, e1, e2){ }
	void unparse(std::ostream& out, int indent) override;
	std::string nodeKind() override { return "Minus"; }
	virtual void typeAnalysis(TypeAnalysis *) override;
	virtual Opd * flatten(Procedure * prog) override;
};

class TimesNode : public BinaryExpNode{
public:
	TimesNode(size_t l, size_t c, ExpNode * e1In, ExpNode * e2In)
	: BinaryExpNode(l, c, e1In, e2In){ }
	void unparse(std::ostream& out, int indent) override;
	std::string nodeKind() override { return "Times"; }
	virtual void typeAnalysis(TypeAnalysis *) override;
	virtual Opd * flatten(Procedure * prog) override;
};

class DivideNode : public BinaryExpNode{
public:
	DivideNode(size_t lIn, size_t cIn, ExpNode * e1, ExpNode * e2)
	: BinaryExpNode(lIn, cIn, e1, e2){ }
	void unparse(std::ostream& out, int indent) override;
	std::string nodeKind() override { return "Divide"; }
	virtual void typeAnalysis(TypeAnalysis *) override;
	virtual Opd * flatten(Procedure * prog) override;
};

class AndNode : public BinaryExpNode{
public:
	AndNode(size_t l, size_t c, ExpNode * e1, ExpNode * e2)
	: BinaryExpNode(l, c, e1, e2){ }
	void unparse(std::ostream& out, int indent) override;
	std::string nodeKind() override { return "And"; }
	virtual void typeAnalysis(TypeAnalysis *) override;
	virtual Opd * flatten(Procedure * prog) override;
};

class OrNode : public BinaryExpNode{
public:
	OrNode(size_t l, size_t c, ExpNode * e1, ExpNode * e2)
	: BinaryExpNode(l, c, e1, e2){ }
	void unparse(std::ostream& out, int indent) override;
	std::string nodeKind() override { return "Or"; }
	virtual void typeAnalysis(TypeAnalysis *) override;
	virtual Opd * flatten(Procedure * prog) override;
};

class EqualsNode : public BinaryExpNode{
public:
	EqualsNode(size_t l, size_t c, ExpNode * e1, ExpNode * e2)
	: BinaryExpNode(l, c, e1, e2){ }
	void unparse(std::ostream& out, int indent) override;
	std::string nodeKind() override { return "Eq"; }
	virtual void typeAnalysis(TypeAnalysis *) override;
	virtual Opd * flatten(Procedure * prog) override;
	
};

class NotEqualsNode : public BinaryExpNode{
public:
	NotEqualsNode(size_t l, size_t c, ExpNode * e1, ExpNode * e2)
	: BinaryExpNode(l, c, e1, e2){ }
	void unparse(std::ostream& out, int indent) override;
	std::string nodeKind() override { return "NotEq"; }
	virtual void typeAnalysis(TypeAnalysis *) override;
	virtual Opd * flatten(Procedure * prog) override;
	
};

class LessNode : public BinaryExpNode{
public:
	LessNode(size_t lineIn, size_t colIn, 
		ExpNode * exp1, ExpNode * exp2)
	: BinaryExpNode(lineIn, colIn, exp1, exp2){ }
	void unparse(std::ostream& out, int indent) override;
	std::string nodeKind() override { return "Less"; }
	virtual void typeAnalysis(TypeAnalysis *) override;
	virtual Opd * flatten(Procedure * proc) override;
};

class LessEqNode : public BinaryExpNode{
public:
	LessEqNode(size_t l, size_t c, ExpNode * e1, ExpNode * e2)
	: BinaryExpNode(l, c, e1, e2){ }
	void unparse(std::ostream& out, int indent) override;
	std::string nodeKind() override { return "LessEq"; }
	virtual void typeAnalysis(TypeAnalysis *) override;
	virtual Opd * flatten(Procedure * prog) override;
};

class GreaterNode : public BinaryExpNode{
public:
	GreaterNode(size_t lineIn, size_t colIn, 
		ExpNode * exp1, ExpNode * exp2)
	: BinaryExpNode(lineIn, colIn, exp1, exp2){ }
	void unparse(std::ostream& out, int indent) override;
	std::string nodeKind() override { return "GreaterEq"; }
	virtual void typeAnalysis(TypeAnalysis *) override;
	virtual Opd * flatten(Procedure * proc) override;
};

class GreaterEqNode : public BinaryExpNode{
public:
	GreaterEqNode(size_t l, size_t c, ExpNode * e1, ExpNode * e2)
	: BinaryExpNode(l, c, e1, e2){ }
	void unparse(std::ostream& out, int indent) override;
	std::string nodeKind() override { return "GreaterEq"; }
	virtual void typeAnalysis(TypeAnalysis *) override;
	virtual Opd * flatten(Procedure * prog) override;
};

class UnaryExpNode : public ExpNode {
public:
	UnaryExpNode(size_t lIn, size_t cIn, ExpNode * expIn) 
	: ExpNode(lIn, cIn){
		this->myExp = expIn;
	}
	virtual void unparse(std::ostream& out, int indent) override = 0;
	virtual bool nameAnalysis(SymbolTable * symTab) override = 0;
	virtual void typeAnalysis(TypeAnalysis *) override = 0;
	virtual Opd * flatten(Procedure * prog) override = 0;
protected:
	ExpNode * myExp;
};

class NegNode : public UnaryExpNode{
public:
	NegNode(size_t l, size_t c, ExpNode * exp)
	: UnaryExpNode(l, c, exp){ }
	void unparse(std::ostream& out, int indent) override;
	std::string nodeKind() override { return "Neg"; }
	bool nameAnalysis(SymbolTable * symTab) override;
	virtual void typeAnalysis(TypeAnalysis *) override;
	virtual Opd * flatten(Procedure * prog) override;
};

class NotNode : public UnaryExpNode{
public:
	NotNode(size_t lIn, size_t cIn, ExpNode * exp)
	: UnaryExpNode(lIn, cIn, exp){ }
	void unparse(std::ostream& out, int indent) override;
	std::string nodeKind() override { return "Not"; }
	bool nameAnalysis(SymbolTable * symTab) override;
	virtual void typeAnalysis(TypeAnalysis *) override;
	virtual Opd * flatten(Procedure * prog) override;
};

class VoidTypeNode : public TypeNode{
public:
	VoidTypeNode(size_t l, size_t c) : TypeNode(l, c){}
	void unparse(std::ostream& out, int indent) override;
	virtual std::string nodeKind() override { return "VoidType"; }
	virtual DataType * getType()override { 
		return BasicType::VOID(); 
	}
	virtual void typeAnalysis(TypeAnalysis *) override;
};

class IntTypeNode : public TypeNode{
public:
	IntTypeNode(size_t l, size_t c, bool ptrIn): TypeNode(l, c), isPtr(ptrIn){}
	void unparse(std::ostream& out, int indent) override;
	virtual std::string nodeKind() override { return "IntType"; }
	virtual DataType * getType() override;
	virtual void typeAnalysis(TypeAnalysis *) override;
private:
	const bool isPtr;
};

class BoolTypeNode : public TypeNode{
public:
	BoolTypeNode(size_t l, size_t c, bool ptrIn): TypeNode(l, c), isPtr(ptrIn) { }
	void unparse(std::ostream& out, int indent) override;
	virtual std::string nodeKind() override { return "BoolType"; }
	virtual DataType * getType() override;
	virtual void typeAnalysis(TypeAnalysis *) override;
private:
	const bool isPtr;
};


class AssignExpNode : public ExpNode{
public:
	AssignExpNode(size_t l, size_t c, LValNode * dstIn, ExpNode * srcIn)
	: ExpNode(l, c), myDst(dstIn), mySrc(srcIn){ }
	void unparse(std::ostream& out, int indent) override;
	virtual std::string nodeKind() override { return "AssignExp"; }
	bool nameAnalysis(SymbolTable * symTab) override;
	virtual void typeAnalysis(TypeAnalysis *) override;
	virtual Opd * flatten(Procedure * proc) override;
private:
	LValNode * myDst;
	ExpNode * mySrc;
};

class IntLitNode : public ExpNode{
public:
	IntLitNode(size_t l, size_t c, const int numIn)
	: ExpNode(l, c), myNum(numIn){ }
	virtual void unparseNested(std::ostream& out) override{
		unparse(out, 0);
	}
	void unparse(std::ostream& out, int indent) override;
	virtual std::string nodeKind() override { return "IntLit"; }
	bool nameAnalysis(SymbolTable * symTab) override;
	virtual void typeAnalysis(TypeAnalysis *) override;
	virtual Opd * flatten(Procedure * prog) override;
private:
	const int myNum;
};

class StrLitNode : public ExpNode{
public:
	StrLitNode(size_t l, size_t c, const std::string strIn)
	: ExpNode(l, c), myStr(strIn){ }
	virtual void unparseNested(std::ostream& out) override{
		unparse(out, 0);
	}
	void unparse(std::ostream& out, int indent) override;
	virtual std::string nodeKind() override { return "StrLit"; }
	bool nameAnalysis(SymbolTable *) override;
	virtual void typeAnalysis(TypeAnalysis *) override;
	virtual Opd * flatten(Procedure * proc) override;
private:
	 const std::string myStr;
};

class CharLitNode : public ExpNode{
public:
	CharLitNode(size_t l, size_t c, const char valIn)
	: ExpNode(l, c), myVal(valIn){ }
	virtual void unparseNested(std::ostream& out) override{
		unparse(out, 0);
	}
	void unparse(std::ostream& out, int indent) override;
	virtual std::string nodeKind() override { return "CharLit"; }
	bool nameAnalysis(SymbolTable *) override;
	virtual void typeAnalysis(TypeAnalysis *) override;
	virtual Opd * flatten(Procedure * proc) override;
private:
	 const char myVal;
};

class NullPtrNode : public ExpNode{
public:
	NullPtrNode(size_t l, size_t c): ExpNode(l, c){ }
	virtual void unparseNested(std::ostream& out) override{
		unparse(out, 0);
	}
	void unparse(std::ostream& out, int indent) override;
	virtual std::string nodeKind() override { return "NullPtr"; }
	bool nameAnalysis(SymbolTable *) override;
	virtual void typeAnalysis(TypeAnalysis *) override;
	virtual Opd * flatten(Procedure * proc) override;
};

class TrueNode : public ExpNode{
public:
	TrueNode(size_t l, size_t c): ExpNode(l, c){ }
	virtual void unparseNested(std::ostream& out) override{
		unparse(out, 0);
	}
	void unparse(std::ostream& out, int indent) override;
	virtual std::string nodeKind() override { return "True"; }
	bool nameAnalysis(SymbolTable * symTab) override;
	virtual void typeAnalysis(TypeAnalysis *) override;
	virtual Opd * flatten(Procedure * prog) override;
};

class FalseNode : public ExpNode{
public:
	FalseNode(size_t l, size_t c): ExpNode(l, c){ }
	virtual void unparseNested(std::ostream& out) override{
		unparse(out, 0);
	}
	void unparse(std::ostream& out, int indent) override;
	virtual std::string nodeKind() override { return "False"; }
	bool nameAnalysis(SymbolTable * symTab) override;
	virtual void typeAnalysis(TypeAnalysis *) override;
	virtual Opd * flatten(Procedure * prog) override;
};

class CallStmtNode : public StmtNode{
public:
	CallStmtNode(size_t l, size_t c, CallExpNode * expIn)
	: StmtNode(l, c), myCallExp(expIn){ }
	void unparse(std::ostream& out, int indent) override;
	std::string nodeKind() override { return "CallStmt"; }
	bool nameAnalysis(SymbolTable * symTab) override;
	virtual void typeAnalysis(TypeAnalysis *) override;
	virtual void to3AC(Procedure * proc) override;
private:
	CallExpNode * myCallExp;
};

} //End namespace holeyc

#endif

