#ifndef HOLEYC_AST_HPP
#define HOLEYC_AST_HPP

#include <ostream>
#include <list>
#include "tokens.hpp"

// **********************************************************************
// ASTnode class (base class for all other kinds of nodes)
// **********************************************************************

namespace holeyc {

/*
 * You may find it useful to forward declare AST subclasses
 * here so that you can use a class before it's full definition
 */

class AndNode;             /* Unimplemented */
class DivideNode;          /* Unimplemented */
class EqualsNode;          /* Unimplemented */
class GreaterEqNode;       /* Unimplemented */
class GreaterNode;         /* Unimplemented */
class LessEqNode;          /* Unimplemented */
class LessNode;            /* Unimplemented */
class MinusNode;           /* Unimplemented */
class NotEqualsNode;       /* Unimplemented */
class OrNode;              /* Unimplemented */
class PlusNode;            /* Unimplemented */
class TimesNode;           /* Unimplemented */
class DerefNode;           /* Unimplemented */
class IDNode;              /* Given */
class IndexNode;           /* Unimplemented */
class RefNode;             /* Unimplemented */
class NegNode;             /* Unimplemented */
class NotNode;             /* Unimplemented */
class FnDeclNode;          /* Unimplemented */
class FormalDeclNode;      /* Unimplemented */
class VarDeclNode;         /* Given */
class AssignExpNode;       /* Unimplemented */
class BinaryExpNode;       /* Unimplemented */
class CallExpNode;         /* Unimplemented */
class CharLitNode;         /* Unimplemented */
class FalseNode;           /* Unimplemented */
class IntLitNode;          /* Unimplemented */
class LValNode;            /* Unimplemented */
class NullPtrNode;         /* Unimplemented */
class StrLitNode;          /* Unimplemented */
class TrueNode;            /* Unimplemented */
class UnaryExpNode;        /* Unimplemented */
class AssignStmtNode;      /* Unimplemented */
class CallStmtNode;        /* Unimplemented */
class DeclNode;            /* Given */
class FromConsoleStmtNode; /* Unimplemented */
class IfElseStmtNode;      /* Unimplemented */
class IfStmtNode;          /* Unimplemented */
class PostDecStmtNode;     /* Unimplemented */
class PostIncStmtNode;     /* Unimplemented */
class ReturnStmtNode;      /* Unimplemented */
class ToConsoleStmtNode;   /* Unimplemented */
class WhileStmtNode;       /* Unimplemented */
class BoolTypeNode;        /* Declared */
class CharTypeNode;        /* Declared */
class IntTypeNode;         /* Given */
class VoidTypeNode;        /* Declared */
class ExpNode;             /* Given */
class ProgramNode;         /* Given */
class StmtNode;            /* Unimplemented */
class TypeNode;            /* Given */
class DeclListNode; // ???

class ASTNode{
public:
	ASTNode(size_t lineIn, size_t colIn)
		: l(lineIn), c(colIn){
	}
	virtual void unparse(std::ostream& out, int indent) = 0;
	size_t line(){ return l; }
	size_t col() { return c; }

	/**
	 * Return a string specifying the position this node begins.
	 * For some node a position doesn't really make sense (i.e.
	 * ProgramNode) but for the rest it's the position in the
	 * input file that represents that node
	 **/
	std::string pos(){
		return "[" + std::to_string(line()) + ","
			+ std::to_string(col()) + "]";
	}

private:
	size_t l; /// The line at which the node starts in the input file
	size_t c; /// The column at which the node starts in the input file
};

class ProgramNode : public ASTNode {
public:
	ProgramNode(std::list<DeclNode*>* globalsIn)
		: ASTNode(1, 1), myGlobals(globalsIn) {
	}
	void unparse(std::ostream& out, int indent) override;
private:
	std::list<DeclNode*>* myGlobals;
};

class ExpNode : public ASTNode {
protected:
	ExpNode(size_t line, size_t col)
		: ASTNode(line, col) {
	}
public:
	void unparse(std::ostream& out, int indent) override = 0; 
};

class BinaryExpNode : public ExpNode {
public:
	BinaryExpNode(size_t line, size_t col, ExpNode* lhs, ExpNode* rhs)
		: ExpNode(line, col), myLhs(lhs), myRhs(rhs) {
	}
	virtual void unparse(std::ostream& out, int indent);
protected:
	ExpNode* myLhs;
	ExpNode* myRhs;
};

class AndNode : public BinaryExpNode {
public:
	AndNode(size_t line, size_t col, ExpNode* lhs, ExpNode* rhs)
		: BinaryExpNode(line, col, lhs, rhs) {
	}
	void unparse(std::ostream& out, int indent);
private:
};

class DivideNode : public BinaryExpNode {
public:
	DivideNode(size_t line, size_t col, ExpNode* lhs, ExpNode* rhs)
		: BinaryExpNode(line, col, lhs, rhs) {
	}
	void unparse(std::ostream& out, int indent);
private:
};

class EqualsNode : public BinaryExpNode {
public:
	EqualsNode(size_t line, size_t col, ExpNode* lhs, ExpNode* rhs)
		: BinaryExpNode(line, col, lhs, rhs) {
	}
	void unparse(std::ostream& out, int indent);
private:
};

class GreaterNode : public BinaryExpNode {
public:
	GreaterNode(size_t line, size_t col, ExpNode* lhs, ExpNode* rhs)
		: BinaryExpNode(line, col, lhs, rhs) {
	}
	void unparse(std::ostream& out, int indent);
private:
};

class GreaterEqNode : public BinaryExpNode {
public:
	GreaterEqNode(size_t line, size_t col, ExpNode* lhs, ExpNode* rhs)
		: BinaryExpNode(line, col, lhs, rhs) {
	}
	void unparse(std::ostream& out, int indent);
private:
};

class LessNode : public BinaryExpNode {
public:
	LessNode(size_t line, size_t col, ExpNode* lhs, ExpNode* rhs)
		: BinaryExpNode(line, col, lhs, rhs) {
	}
	void unparse(std::ostream& out, int indent);
private:
};

class LessEqNode : public BinaryExpNode {
public:
	LessEqNode(size_t line, size_t col, ExpNode* lhs, ExpNode* rhs)
		: BinaryExpNode(line, col, lhs, rhs) {
	}
	void unparse(std::ostream& out, int indent);
private:
};

class MinusNode : public BinaryExpNode {
public:
	MinusNode(size_t line, size_t col, ExpNode* lhs, ExpNode* rhs)
		: BinaryExpNode(line, col, lhs, rhs) {
	}
	void unparse(std::ostream& out, int indent);
private:
};

class NotEqualsNode : public BinaryExpNode {
public:
	NotEqualsNode(size_t line, size_t col, ExpNode* lhs, ExpNode* rhs)
		: BinaryExpNode(line, col, lhs, rhs) {
	}
	void unparse(std::ostream& out, int indent);
private:
};

class OrNode : public BinaryExpNode {
public:
	OrNode(size_t line, size_t col, ExpNode* lhs, ExpNode* rhs)
		: BinaryExpNode(line, col, lhs, rhs) {
	}
	void unparse(std::ostream& out, int indent);
private:
};

class PlusNode : public BinaryExpNode {
public:
	PlusNode(size_t line, size_t col, ExpNode* lhs, ExpNode* rhs)
		: BinaryExpNode(line, col, lhs, rhs) {
	}
	void unparse(std::ostream& out, int indent);
private:
};

class TimesNode : public BinaryExpNode {
public:
	TimesNode(size_t line, size_t col, ExpNode* lhs, ExpNode* rhs)
		: BinaryExpNode(line, col, lhs, rhs) {
	}
	void unparse(std::ostream& out, int indent);
private:
};

class UnaryExpNode : public ExpNode {
public:
	UnaryExpNode(size_t line, size_t col, ExpNode* expr)
		: ExpNode(line, col), myExpr(expr) {
	}
	void unparse(std::ostream& out, int indent);
protected:
	ExpNode* myExpr;
};

class NegNode : public UnaryExpNode {
public:
	NegNode(size_t line, size_t col, ExpNode* expr)
		: UnaryExpNode(line, col, expr) {
	}
	void unparse(std::ostream& out, int indent);
private:
};

class NotNode : public UnaryExpNode {
public:
	NotNode(size_t line, size_t col, ExpNode* expr)
		: UnaryExpNode(line, col, expr) {
	}
	void unparse(std::ostream& out, int indent);
private:
};
/*
 * A variable declaration. Note that this class is intended to
 * represent a global or local variable of any type (including a struct
 * type. Note that this is not intended to represent a declaration of
 * a struct. In other words:
 * struct MyStruct {
 *   int fieldA;
 * };
 * is NOT a VarDeclNode because it introduces a new datatype, not a new
 * variable (in this case, the example is a StructDeclNode).  * However,
 * struct MyStruct instance; *is* a VarDeclNode, since it introduces a
 * new variable to the program.
 */
class TypeNode : public ASTNode {
protected:
	TypeNode(size_t lineIn, size_t colIn, bool refIn)
		: ASTNode(lineIn, colIn), myIsReference(refIn){
	}
public:
	virtual void unparse(std::ostream& out, int indent) = 0;
private:
	bool myIsReference;
};

class LValNode : public ExpNode {
public:
	LValNode(size_t lineIn, size_t colIn, IDNode* id, ExpNode* expr)
		: ExpNode(lineIn, colIn), myId(id), myExp(expr) {
	}
	void unparse(std::ostream& out, int indent);
protected:
	IDNode* myId;
	ExpNode* myExp;
};

class IDNode : public LValNode {
public:
	IDNode(IDToken* token)
		: LValNode(token->line(), token->col()), myStrVal(token->value()) {
		myStrVal = token->value();
	}
	void unparse(std::ostream& out, int indent);
private:
	/** The name of the identifier **/
	std::string myStrVal;
};


class DerefNode : public LValNode {
public:
	DerefNode(size_t lineIn, size_t colIn, IDNode* id, ExpNode* expr)
		: LValNode(lineIn, colIn, id, expr) {
	}
	void unparse(std::ostream& out, int indent);
private:
};

class RefNode : public LValNode {
public:
	RefNode(size_t lineIn, size_t colIn, IDNode* id, ExpNode* expr)
		: LValNode(lineIn, colIn, id, expr) {
	}
	void unparse(std::ostream& out, int indent);
private:
};

class AssignExpNode : public ExpNode {
public:
	AssignExpNode(size_t lineIn, size_t colIn, LValNode* lval, ExpNode* srcExpr)
		: ExpNode(lineIn, colIn), myLval(lval), mySrcExpr(srcExpr) {
	}
	void unparse(std::ostream& out, int indent);
private:
	LValNode* myLval;
	ExpNode* mySrcExpr;
};

class CallExpNode : public ExpNode {
public:
	CallExpNode(size_t lineIn, size_t colIn, IDNode* id, std::list<ExpNode*>* actuals) 
	    : ExpNode(lineIn, colIn), myId(id), myActuals(actuals) {
	}
	void unparse(std::ostream& out, int indent);
private:
	IDNode* myId;
	std::list<ExpNode*>* myActuals;
};

class StmtNode : public ASTNode {
public:
	StmtNode(size_t lineIn, size_t colIn)
		: ASTNode(lineIn, colIn) {
	}
	void unparse(std::ostream& out, int indent); 
private:
};

class AssignStmtNode : public StmtNode {
public:
	AssignStmtNode(size_t lineIn, size_t colIn, AssignExpNode* expr)
		: StmtNode(lineIn, colIn), myExpr(expr) {
	}
	void unparse(std::ostream& out, int indent);
private:
	AssignExpNode* myExpr;
};

class FromConsoleStmtNode : public StmtNode {
public:
	FromConsoleStmtNode(size_t lineIn, size_t colIn, ExpNode* expr)
		: StmtNode(lineIn, colIn), myExpr(expr) {
	}
	void unparse(std::ostream& out, int indent);
private:
	ExpNode* myExpr;
};

class ToConsoleStmtNode : public StmtNode {
public:
	ToConsoleStmtNode(size_t lineIn, size_t colIn, ExpNode* expr) 
		: StmtNode(lineIn, colIn), myExpr(expr) {
	}
	void unparse(std::ostream& out, int indent);
private:
	ExpNode* myExpr;
};

class PostDecStmtNode : public StmtNode {
public:
	PostDecStmtNode(size_t lineIn, size_t colIn, ExpNode* expr) 
		: StmtNode(lineIn, colIn), myExpr(expr) {
	}
	void unparse(std::ostream& out, int indent);
private:
	ExpNode* myExpr;
};

class PostIncStmtNode : public StmtNode {
public:
	PostIncStmtNode(size_t lineIn, size_t colIn, ExpNode* expr) 
		: StmtNode(lineIn, colIn), myExpr(expr) {
	}
	void unparse(std::ostream& out, int indent);
private:
	ExpNode* myExpr;
};

class DeclNode : public StmtNode {
public:
	DeclNode(size_t line, size_t col) : StmtNode(line, col) {
	}
	void unparse(std::ostream& out, int indent) override = 0;
};

class VarDeclNode : public DeclNode {
public:
	VarDeclNode(size_t l, size_t c, TypeNode * type, IDNode * id)
		: DeclNode(type->line(), type->col()), myType(type), myId(id) {
	}
	void unparse(std::ostream& out, int indent);
private:
	TypeNode * myType;
	IDNode * myId;
};

class FormalDeclNode : public DeclNode {
public:
	FormalDeclNode(TypeNode* type, IDNode* id)
		: DeclNode(type->line(), type->col()), myType(type), myId(id) {
	}
	void unparse(std::ostream& out, int indent);
private:
	TypeNode* myType;
	IDNode* myId;
};

class FnDeclNode : public DeclNode {
public:
	FnDeclNode(TypeNode* type, IDNode* id, std::list<FormalDeclNode*>* formalsList, std::list<StmtNode*>* body)
		: DeclNode(type->line(), type->col()),
		  myReturnType(type),
		  myId(id),
		  myFormals(formalsList),
		  myBody(body) {
	}
	void unparse(std::ostream& out, int indent);
private:
	TypeNode* myReturnType;
	IDNode* myId;
	std::list<FormalDeclNode*>* myFormals;
	std::list<StmtNode*>* myBody;
};

class IntTypeNode : public TypeNode {
public:
	IntTypeNode(size_t lineIn, size_t colIn, bool isRefIn)
		: TypeNode(lineIn, colIn, isRefIn){
		this->isRef = isRefIn;
	}
	void unparse(std::ostream& out, int indent);
private:
	bool isRef;
};

class CharTypeNode : public TypeNode{
public:
	CharTypeNode(size_t lineIn, size_t colIn, bool isRefIn)
		: TypeNode(lineIn, colIn, isRefIn){
		this->isRef = isRefIn;
	}
	void unparse(std::ostream& out, int indent);
private:
	bool isRef;
};


class BoolTypeNode : public TypeNode{
public:
	BoolTypeNode(size_t lineIn, size_t colIn, bool isRefIn)
		: TypeNode(lineIn, colIn, isRefIn){
		this->isRef = isRefIn;
	}
	void unparse(std::ostream& out, int indent);
private:
	bool isRef;
};


class VoidTypeNode : public TypeNode{
public:
	VoidTypeNode(size_t lineIn, size_t colIn, bool isRefIn)
		: TypeNode(lineIn, colIn, isRefIn){
	}
	void unparse(std::ostream& out, int indent);
};

} //End namespace holeyc

#endif
