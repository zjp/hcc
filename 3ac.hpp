#ifndef HOLEYC_3AC_HPP
#define HOLEYC_3AC_HPP

#include <assert.h>
#include "list"
#include "map"
#include "err.hpp"
#include "symbol_table.hpp"

namespace holeyc{

class TypeAnalysis;
class Procedure;
class IRProgram;

class Label{
public:
	Label(std::string nameIn){
		this->name = nameIn;
	}
	std::string toString(){
		return "lbl_" + this->name;
	}
private:
	std::string name;
};

enum OpdWidth{
	ADDR, QUADWORD, BYTE
};

class Opd{
public:
	Opd(OpdWidth widthIn) : myWidth(widthIn){}
	virtual std::string toString() = 0;
	virtual OpdWidth getWidth(){ return myWidth; }
	static OpdWidth width(const DataType * type){
		if (const BasicType * basic = type->asBasic()){
			if (basic->isChar()){ return BYTE; }
			return QUADWORD;
		} else if (const PtrType * ptr = type->asPtr()){
			return ADDR;
		}
		assert(false);
	}
private:
	OpdWidth myWidth;
};

class SymOpd : public Opd{
public:
	SymOpd(OpdWidth width) : Opd(width){}
	virtual std::string toString() override{
		return mySym->getName();
	}
	const SemSymbol * getSym(){ return mySym; }
private:
	//Private Constructor
	SymOpd(SemSymbol * sym, OpdWidth width)
	: Opd(width), mySym(sym) {} 
	SemSymbol * mySym;
	friend class Procedure;
	friend class IRProgram;
};

class LitOpd : public holeyc::Opd{
public:
	LitOpd(std::string valIn, OpdWidth width)
	: Opd(width), val(valIn){ }
	std::string toString() override{
		return val;
	}
private:
	std::string val;
};

class AuxOpd : public Opd{
public:
	AuxOpd(std::string valIn, OpdWidth width) 
	: Opd(width), val(valIn) { }
	std::string toString() override{
		return val;
	}
private:
	std::string val;
};

enum BinOp {
	ADD, SUB, DIV, MULT, OR, AND, EQ, NEQ, LT, GT, LTE, GTE
};
enum UnaryOp{
	NEG, NOT
};
enum Intrinsic {
	OUTPUT, INPUT
};

class Quad{
public:
	Quad();
	void addLabel(Label * label);
	Label * getLabel(){ return labels.front(); }
	virtual std::string repr() = 0;
	std::string commentStr();
	virtual std::string toString(bool verbose=false);
	void setComment(std::string commentIn);
private:
	std::string myComment;
	std::list<Label *> labels;
};

class BinOpQuad : public Quad{
public:
	BinOpQuad(Opd * dstIn, BinOp opIn, Opd * src1In, Opd * src2In);
	std::string repr() override;
private:
	Opd * dst;
	BinOp op;
	Opd * src1;
	Opd * src2;
};

class UnaryOpQuad : public Quad {
public:
	UnaryOpQuad(Opd * dstIn, UnaryOp opIn, Opd * srcIn);
	std::string repr() override ;
private:
	Opd * dst;
	UnaryOp op;
	Opd * src;
};

class AssignQuad : public Quad{
	
public:
	AssignQuad(Opd * dstIn, Opd * srcIn)
	: dst(dstIn), src(srcIn)
	{ }
	std::string repr() override;

private:
	Opd * dst;
	Opd * src;
};

class LocQuad : public Quad {
public:
	LocQuad(Opd * srcIn, Opd * tgtIn);
	std::string repr() override;
private:
	Opd * src;
	Opd * tgt;
};

class JmpQuad : public Quad {
public:
	JmpQuad(Label * tgtIn);
	std::string repr() override;
private:
	Label * tgt;
};

class JmpIfQuad : public Quad {
public:
	JmpIfQuad(Opd * cndIn, Label * tgtIn);
	std::string repr() override;
private:
	Opd * cnd;
	Label * tgt;
};

class NopQuad : public Quad {
public:
	NopQuad();
	std::string repr() override;
};

class IntrinsicQuad : public Quad {
public:
	IntrinsicQuad(Intrinsic intrinsic, Opd * arg);
	std::string repr() override;
private:
	Opd * myArg;
	Intrinsic myIntrinsic;
};

class CallQuad : public Quad{
public:
	CallQuad(SemSymbol * calleeIn);
	std::string repr() override;
private:
	SemSymbol * callee;
};

class EnterQuad : public Quad{
public:
	EnterQuad(Procedure * proc);
	virtual std::string repr() override;
private:
	Procedure * myProc;
};

class LeaveQuad : public Quad{
public:
	LeaveQuad(Procedure * proc);
	virtual std::string repr() override;
private:
	Procedure * myProc;
};

class SetArgQuad : public Quad{
public:
	SetArgQuad(size_t indexIn, Opd * opdIn);
	std::string repr() override;
private:
	size_t index;
	Opd * opd;
};

class GetArgQuad : public Quad{
public:
	GetArgQuad(size_t indexIn, Opd * opdIn);
	std::string repr() override;
private:
	size_t index;
	Opd * opd;
};

class SetRetQuad : public Quad{
public:
	SetRetQuad(Opd * opdIn);
	std::string repr() override;
private:
	size_t index;
	Opd * opd;
};

class GetRetQuad : public Quad{
public:
	GetRetQuad(Opd * opdIn);
	std::string repr() override;
private:
	size_t index;
	Opd * opd;
};

class Procedure{
public:
	Procedure(IRProgram * prog, std::string name);
	void addQuad(Quad * quad);
	Quad * popQuad();
	IRProgram * getProg();
	std::list<SymOpd *> getFormals() { return formals; }
	holeyc::Label * makeLabel();

	void gatherLocal(SemSymbol * sym);
	void gatherFormal(SemSymbol * sym);
	SymOpd * getSymOpd(SemSymbol * sym);
	AuxOpd * makeTmp(OpdWidth width);

	std::string toString(bool verbose=false); 
	std::string getName();

	holeyc::Label * getLeaveLabel();
private:
	EnterQuad * enter;
	Quad * leave;
	Label * leaveLabel;

	IRProgram * myProg;
	std::map<SemSymbol *, SymOpd *> locals;
	std::list<AuxOpd *> temps; 
	std::list<SymOpd *> formals; 
	std::list<Quad *> bodyQuads;
	std::string myName;
	size_t maxTmp;
};

class IRProgram{
public:
	IRProgram(TypeAnalysis * taIn) : ta(taIn){}
	Procedure * makeProc(std::string name);
	Label * makeLabel();
	Opd * makeString(std::string val);
	void gatherGlobal(SemSymbol * sym);
	SymOpd * getGlobal(SemSymbol * sym);
	OpdWidth opWidth(ASTNode * node);

	std::string toString(bool verbose=false);
private:
	TypeAnalysis * ta;
	size_t max_label = 0;
	size_t str_idx = 0;
	std::list<Procedure *> procs; 
	HashMap<AuxOpd *, std::string> strings;
	std::map<SemSymbol *, SymOpd *> globals;
};

}

#endif 
