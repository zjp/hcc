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
class MultNode;            /* Unimplemented */
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
	std::list<DeclNode * > * myGlobals;
};

class DeclNode : public ASTNode {
public:
	DeclNode(size_t line, size_t col) : ASTNode(line, col) {
	}
	void unparse(std::ostream& out, int indent) override = 0;
};

class ExpNode : public ASTNode {
protected:
	ExpNode(size_t line, size_t col)
		: ASTNode(line, col){
	}
	void unparse(std::ostream& out, int indent) override = 0;
};

class IDNode : public ExpNode {
public:
	IDNode(IDToken * token)
		: ExpNode(token->line(), token->col()), myStrVal(token->value()) {
		myStrVal = token->value();
	}
	void unparse(std::ostream& out, int indent);
private:
	/** The name of the identifier **/
	std::string myStrVal;
};

class BinaryExpNode : public ExpNode {
	protected:
	ExpNode(size_t line, size_t col)
		: ASTNode(line, col){
	}
    ExpNode * myArm1, myArm2;
	void unparse(std::ostream& out, int indent) override = 0;
};

class AndNode : public BinaryExpNode {
public:
	virtual void unparse(std::ostream& out, int indent);
private:
    ExpNode * myArm1, myArm2;
};

class DivideNode : public BinaryExpNode {
public:
	virtual void unparse(std::ostream& out, int indent);
private:
    ExpNode * myArm1, myArm2;
};
class AndNode : public BinaryExpNode {
public:
	virtual void unparse(std::ostream& out, int indent);
private:
    ExpNode * myArm1, myArm2;
};

class EqualsNode : public BinaryExpNode {
public:
	virtual void unparse(std::ostream& out, int indent);
private:
    ExpNode * myArm1, myArm2;
};

class GreaterNode : public BinaryExpNode {
public:
	virtual void unparse(std::ostream& out, int indent);
private:
    ExpNode * myArm1, myArm2;
};

class GreaterEqNode : public BinaryExpNode {
public:
	virtual void unparse(std::ostream& out, int indent);
private:
    ExpNode * myArm1, myArm2;
};

class LessNode : public BinaryExpNode {
public:
	virtual void unparse(std::ostream& out, int indent);
private:
    ExpNode * myArm1, myArm2;
};

class LessEqNode : public BinaryExpNode {
public:
	virtual void unparse(std::ostream& out, int indent);
private:
    ExpNode * myArm1, myArm2;
};

class MinusNode : public BinaryExpNode {
public:
	virtual void unparse(std::ostream& out, int indent);
private:
    ExpNode * myArm1, myArm2;
};

class NotEqualsNode : public BinaryExpNode {
public:
	virtual void unparse(std::ostream& out, int indent);
private:
    ExpNode * myArm1, myArm2;
};

class OrNode : public BinaryExpNode {
public:
	virtual void unparse(std::ostream& out, int indent);
private:
    ExpNode * myArm1, myArm2;
};

class PlusNode : public BinaryExpNode {
public:
	virtual void unparse(std::ostream& out, int indent);
private:
    ExpNode * myArm1, myArm2;
};

class TimesNode : public BinaryExpNode {
public:
	virtual void unparse(std::ostream& out, int indent);
private:
    ExpNode * myArm1, myArm2;
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
class TypeNode : public ASTNode{
protected:
	TypeNode(size_t lineIn, size_t colIn, bool refIn)
		: ASTNode(lineIn, colIn), myIsReference(refIn){
	}
public:
	virtual void unparse(std::ostream& out, int indent) = 0;
	//TODO: consider adding an isRef to use in unparse to
	// indicate if this is a reference type
private:
	bool myIsReference;
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

class FnDeclNode : public DeclNode {
public:
	FnDeclNode(size_t l, size_t c, TypeNode* type, IDNode* id)
		: DeclNode(type->line(), type->col()), myType(type), myId(id) {
	}
	void unparse(std::ostream& out, int indent);
private:
	TypeNode* myType;
	IDNode* myId;
};

class IntTypeNode : public TypeNode{
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
