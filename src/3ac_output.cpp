#include <iterator> 

#include "ast.hpp"

#define MAKEBINOP(OPERATOR) \
	Opd* tmp1 = myExp1->flatten(proc);\
	Opd* tmp2 = myExp2->flatten(proc);\
	Opd* dst = proc->makeTmp(QUADWORD);\
	BinOpQuad* b = new BinOpQuad(dst, OPERATOR, tmp1, tmp2);\
	proc->addQuad(b);\
	return dst;

#define MAKEVARWIDTHBINOP(OPERATOR) \
	Opd* tmp1 = myExp1->flatten(proc);\
	Opd* tmp2 = myExp2->flatten(proc);\
	Opd* dst = proc->makeTmp(tmp1->getWidth());\
	BinOpQuad* b = new BinOpQuad(dst, OPERATOR, tmp1, tmp2);\
	proc->addQuad(b);\
	return dst;

#define MAKEUNOP(OPERATOR) \
	Opd* dst = proc->makeTmp(QUADWORD);\
	Opd* tmp = myExp->flatten(proc);\
	UnaryOpQuad* u = new UnaryOpQuad(dst, OPERATOR, tmp);\
	proc->addQuad(u);\
	return dst;

#define MAKELITOPD(VALUE, TYPE)\
	return new LitOpd(VALUE, TYPE);

#define MAKEINTRINSICQUAD(TYPE, OP)\
	return new LitOpd

namespace holeyc{

IRProgram * ProgramNode::to3AC(TypeAnalysis * ta){
	IRProgram * prog = new IRProgram(ta);
	for (auto global : *myGlobals){
		global->to3AC(prog);
	}
	return prog;
}

void FnDeclNode::to3AC(IRProgram * prog){
    Procedure *p  = prog->makeProc(myID->getName()); 
	size_t formalNumber = 0;
    for (auto child : *myFormals){
		child->to3AC(p);
		p->addQuad(
				new GetArgQuad(
					static_cast<size_t>(std::distance(*myFormals->begin(), child))
					, p->getSymOpd(child->ID()->getSymbol())
					)
				);
	}
}

/* Do Not Implement */
void FnDeclNode::to3AC(Procedure * proc){
}

/* Do Not Implement */
void FormalDeclNode::to3AC(IRProgram * prog){
}

void FormalDeclNode::to3AC(Procedure * proc){
	proc->gatherFormal(myID->getSymbol());
}

Opd * IntLitNode::flatten(Procedure * proc){
	MAKELITOPD(std::to_string(myNum), QUADWORD);
}

Opd * StrLitNode::flatten(Procedure * proc){
	// MAKELITOPD(proc->getProg()->makeString(myStr), ??)
	Opd * res = proc->getProg()->makeString(myStr);
	return res;
}

Opd * CharLitNode::flatten(Procedure * proc){
	MAKELITOPD(std::to_string(myVal), BYTE);
}

Opd * NullPtrNode::flatten(Procedure * proc){
	MAKELITOPD("0", ADDR);
}

Opd * TrueNode::flatten(Procedure * prog){
	MAKELITOPD("1", BYTE);
}

Opd * FalseNode::flatten(Procedure * prog){
	MAKELITOPD("0", BYTE);
}

Opd * AssignExpNode::flatten(Procedure * proc){
	TODO(Implement me)
}

Opd * LValNode::flatten(Procedure * proc){
	TODO(Implement me)
}

Opd * DerefNode::flatten(Procedure * proc){
	TODO(Implement me)
}

Opd * RefNode::flatten(Procedure * proc){
	TODO(Implement me)
}

Opd * CallExpNode::flatten(Procedure * proc){
	TODO(Implement me)
}

Opd * NegNode::flatten(Procedure * proc){
	MAKEUNOP(NEG);
}

Opd * NotNode::flatten(Procedure * proc){
	MAKEUNOP(NOT);
}

Opd * PlusNode::flatten(Procedure * proc){
	MAKEBINOP(ADD)
}

Opd * MinusNode::flatten(Procedure * proc){
	MAKEBINOP(SUB)
}

Opd * TimesNode::flatten(Procedure * proc){
	MAKEBINOP(MULT)
}

Opd * DivideNode::flatten(Procedure * proc){
	MAKEBINOP(DIV)
}

Opd * AndNode::flatten(Procedure * proc){
	MAKEBINOP(AND)
}

Opd * OrNode::flatten(Procedure * proc){
	MAKEBINOP(OR)
}

Opd * EqualsNode::flatten(Procedure * proc){
	MAKEVARWIDTHBINOP(EQ)
}

Opd * NotEqualsNode::flatten(Procedure * proc){
	MAKEVARWIDTHBINOP(NEQ)
}

Opd * LessNode::flatten(Procedure * proc){
	MAKEBINOP(LT)
}

Opd * GreaterNode::flatten(Procedure * proc){
	MAKEBINOP(GT)
}

Opd * LessEqNode::flatten(Procedure * proc){
	MAKEBINOP(LTE)
}

Opd * GreaterEqNode::flatten(Procedure * proc){
	MAKEBINOP(GTE)
}

void AssignStmtNode::to3AC(Procedure * proc){
	TODO(Implement me)
	//myExp->flatten(proc);
}

void PostIncStmtNode::to3AC(Procedure * proc){
	TODO(Implement me)
}

void PostDecStmtNode::to3AC(Procedure * proc){
	TODO(Implement me)
}

void FromConsoleStmtNode::to3AC(Procedure * proc){
	TODO(Implement me)
}

void ToConsoleStmtNode::to3AC(Procedure * proc){
	TODO(Implement me)
}

void IfStmtNode::to3AC(Procedure * proc){
	TODO(Implement me)
}

void IfElseStmtNode::to3AC(Procedure * proc){
	TODO(Implement me)
}

void WhileStmtNode::to3AC(Procedure * proc){
	TODO(Implement me)
}

void CallStmtNode::to3AC(Procedure * proc){
	TODO(Implement me)
}

void ReturnStmtNode::to3AC(Procedure * proc){
	Opd* ret = myExp->flatten(proc);
	SetRetQuad* r = new SetRetQuad(ret);
	proc->addQuad(r);
}

void VarDeclNode::to3AC(Procedure * proc){
	SemSymbol * sym = ID()->getSymbol();
	if (sym == nullptr){
		throw new InternalError("null sym");
	}
	proc->gatherLocal(sym);
}

void VarDeclNode::to3AC(IRProgram * prog){
	SemSymbol * sym = ID()->getSymbol();
	if (sym == nullptr){
		throw new InternalError("null sym");
	}
	
	prog->gatherGlobal(sym);
}

//We only get to this node if we are in a stmt
// context (DeclNodes protect descent) 
Opd * IDNode::flatten(Procedure * proc){
	TODO(Implement me)
}


}
