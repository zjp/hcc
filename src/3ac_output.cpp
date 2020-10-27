#include "ast.hpp"

namespace holeyc{

IRProgram * ProgramNode::to3AC(TypeAnalysis * ta){
	IRProgram * prog = new IRProgram(ta);
	for (auto global : *myGlobals){
		global->to3AC(prog);
	}
	return prog;
}

void FnDeclNode::to3AC(IRProgram * prog){
	TODO(Implement me)
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
	return new LitOpd(std::to_string(myNum), QUADWORD);
}

Opd * StrLitNode::flatten(Procedure * proc){
	Opd * res = proc->getProg()->makeString(myStr);
	return res;
}

Opd * CharLitNode::flatten(Procedure * proc){
	TODO(Implement me)
}

Opd * NullPtrNode::flatten(Procedure * proc){
	return new LitOpd("0", ADDR);
	TODO(Implement me)
}

Opd * TrueNode::flatten(Procedure * prog){
	TODO(Implement me)
}

Opd * FalseNode::flatten(Procedure * prog){
	TODO(Implement me)
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
	TODO(Implement me)
}

Opd * NotNode::flatten(Procedure * proc){
	TODO(Implement me)
}

Opd * PlusNode::flatten(Procedure * proc){
	TODO(Implement me)
}

Opd * MinusNode::flatten(Procedure * proc){
	TODO(Implement me)
}

Opd * TimesNode::flatten(Procedure * proc){
	TODO(Implement me)
}

Opd * DivideNode::flatten(Procedure * proc){
	TODO(Implement me)
}

Opd * AndNode::flatten(Procedure * proc){
	TODO(Implement me)
}

Opd * OrNode::flatten(Procedure * proc){
	TODO(Implement me)
}

Opd * EqualsNode::flatten(Procedure * proc){
	TODO(Implement me)
}

Opd * NotEqualsNode::flatten(Procedure * proc){
	TODO(Implement me)
}

Opd * LessNode::flatten(Procedure * proc){
	TODO(Implement me)
}

Opd * GreaterNode::flatten(Procedure * proc){
	TODO(Implement me)
}

Opd * LessEqNode::flatten(Procedure * proc){
	TODO(Implement me)
}

Opd * GreaterEqNode::flatten(Procedure * proc){
	TODO(Implement me)
}

void AssignStmtNode::to3AC(Procedure * proc){
	TODO(Implement me)
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
	TODO(Implement me)
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
