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
	Opd* tmp = myExp->flatten(proc);\
    Opd* dst = proc->makeTmp(QUADWORD);\
    UnaryOpQuad* b = new UnaryOpQuad(dst, OPERATOR, tmp);\
    proc->addQuad(b);\
    return dst;

#define MAKELITOPD(VALUE, TYPE)\
	return new LitOpd(VALUE, TYPE);

namespace holeyc{

IRProgram * ProgramNode::to3AC(TypeAnalysis * ta){
	IRProgram * prog = new IRProgram(ta);
	for (auto global : *myGlobals){
		global->to3AC(prog);
	}
	return prog;
}

//p->addQuad(new GetArgQuad(static_cast<size_t>(std::distance(*myFormals->begin(), child)+1)
void FnDeclNode::to3AC(IRProgram * prog){
	Procedure *p = prog->makeProc(myID->getName());
	size_t formNo = 1;
	for(auto child : *myFormals) {
		child->to3AC(p);
		p->addQuad(new GetArgQuad(formNo, p->getSymOpd(child->ID()->getSymbol())));
		++formNo;
	}
	for(auto stmt : * myBody) {
		stmt->to3AC(p);
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
	Opd * res = proc->getProg()->makeString(myStr);
	return res;
}

Opd * CharLitNode::flatten(Procedure * proc){
	MAKELITOPD(std::to_string(myVal), QUADWORD);
}

Opd * NullPtrNode::flatten(Procedure * proc){
	MAKELITOPD("0", ADDR);
}

Opd * TrueNode::flatten(Procedure * prog){
	MAKELITOPD("1", BYTE);
}

Opd * FalseNode::flatten(Procedure * prog){
	MAKELITOPD("0", BYTE)
}

Opd * AssignExpNode::flatten(Procedure * proc){
	Opd* src = mySrc->flatten(proc);
	Opd* dst = myDst->flatten(proc);
	proc->addQuad(new AssignQuad(dst, src));
	return dst;
}

/* Do not Implement and make pure */
//Opd * LValNode::flatten(Procedure * proc){
//}

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
	MAKEUNOP(NEG)
}

Opd * NotNode::flatten(Procedure * proc){
	MAKEUNOP(NOT)
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
	myExp->flatten(proc);
}

void PostIncStmtNode::to3AC(Procedure * proc){
	Opd* dst = myLVal->flatten(proc);
	proc->addQuad(new BinOpQuad(dst, ADD, dst, new LitOpd("1", QUADWORD)));
}

void PostDecStmtNode::to3AC(Procedure * proc){
	Opd* dst = myLVal->flatten(proc);
	proc->addQuad(new BinOpQuad(dst, SUB, dst, new LitOpd("1", QUADWORD)));
}

void FromConsoleStmtNode::to3AC(Procedure * proc){
}

void ToConsoleStmtNode::to3AC(Procedure * proc){
}

void IfStmtNode::to3AC(Procedure * proc){
	Opd* cond = myCond->flatten(proc);
	Label* label1 = proc->makeLabel();
	JmpIfQuad* jif = new JmpIfQuad(cond, label1);
	NopQuad* nop1 = new NopQuad();
	nop1->addLabel(label1);
	proc->addQuad(jif);
	for(auto stmt : *myBody) {
		stmt->to3AC(proc);
	}
	proc->addQuad(nop1);
}

void IfElseStmtNode::to3AC(Procedure * proc){
	Opd* cond = myCond->flatten(proc);
	Label* label1 = proc->makeLabel();
	Label* label2 = proc->makeLabel();
	JmpIfQuad* jif = new JmpIfQuad(cond, label1);
	proc->addQuad(jif);
	for(auto stmt : *myBodyTrue) {
		stmt->to3AC(proc);
	}
	proc->addQuad(new JmpQuad(label2));
	NopQuad* nop1 = new NopQuad();
	nop1->addLabel(label1);
	proc->addQuad(nop1);
	for(auto stmt : *myBodyFalse) {
		stmt->to3AC(proc);
	}
	NopQuad* nop2 = new NopQuad();
	nop2->addLabel(label2);
	proc->addQuad(nop2);
}

void WhileStmtNode::to3AC(Procedure * proc){
	Label* label1 = proc->makeLabel();
	Label* label2 = proc->makeLabel();
	NopQuad* nop1 = new NopQuad();
	nop1->addLabel(label1);
	proc->addQuad(nop1);
	Opd* cond = myCond->flatten(proc);
	JmpIfQuad* jif = new JmpIfQuad(cond, label2);
	proc->addQuad(jif);
	for(auto stmt : *myBody) {
		stmt->to3AC(proc);
	}
	JmpQuad* jmp = new JmpQuad(label1);
	proc->addQuad(jmp);
	NopQuad* nop2 = new NopQuad();
	nop2->addLabel(label2);
	proc->addQuad(nop2);
}

void CallStmtNode::to3AC(Procedure * proc){
}

void ReturnStmtNode::to3AC(Procedure * proc){
	Opd* ret = myExp->flatten(proc);
	SetRetQuad* r = new SetRetQuad(ret);
	JmpQuad* j = new JmpQuad(proc->getLeaveLabel());
	proc->addQuad(r);
	proc->addQuad(j);
}

void VarDeclNode::to3AC(Procedure * proc){
	SemSymbol * sym = ID()->getSymbol();
	if (sym == nullptr){
		throw new InternalError("null sym");
	}
	proc->gatherLocal(sym);
}

void VarDeclNode::to3AC(IRProgram * proc){
	SemSymbol * sym = ID()->getSymbol();
	if (sym == nullptr){
		throw new InternalError("null sym");
	}
	
	proc->gatherGlobal(sym);
}

//We only get to this node if we are in a stmt
// context (DeclNodes protect descent) 
Opd * IDNode::flatten(Procedure * proc){
	return proc->getSymOpd(mySymbol);
}


}
