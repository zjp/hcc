#include "ast.hpp"

namespace holeyc{

IRProgram * ProgramNode::to3AC(TypeAnalysis * ta){
	IRProgram * prog = new IRProgram(ta);
	for (auto global : *myGlobals){
		global->to3AC(prog);
	}
	return prog;
}

static void formalsTo3AC(Procedure * proc, 
  std::list<FormalDeclNode *> * myFormals){
	for (auto formal : *myFormals){
		formal->to3AC(proc);
	}
	unsigned int argIdx = 1;
	for (auto formal : *myFormals){
		SemSymbol * sym = formal->ID()->getSymbol();
		SymOpd * opd = proc->getSymOpd(sym);
		
		Quad * inQuad = new GetArgQuad(argIdx, opd);
		proc->addQuad(inQuad);
		argIdx += 1;
	}
}

void FnDeclNode::to3AC(IRProgram * prog){
	SemSymbol * mySym = this->ID()->getSymbol();
	Procedure * proc = prog->makeProc(mySym->getName());

	//Generate the getin quads
	formalsTo3AC(proc, myFormals);

	for (auto stmt : *myBody){
		stmt->to3AC(proc);
	}
}

void FnDeclNode::to3AC(Procedure * proc){
	throw new InternalError("FnDecl at a local scope");
}

//We only get to this node if we are in a stmt
// context (DeclNodes protect descent) 
Opd * IDNode::flatten(Procedure * proc){
	SemSymbol * sym = this->getSymbol();
	Opd * res = proc->getSymOpd(sym);
	if (!res){
		throw new InternalError("null id sym");;
	}
	return res;
}

void FormalDeclNode::to3AC(IRProgram * prog){
	throw new InternalError("Formal at a global level");
}

void FormalDeclNode::to3AC(Procedure * proc){
	SemSymbol * sym = ID()->getSymbol();
	proc->gatherFormal(sym);
}

Opd * IntLitNode::flatten(Procedure * proc){
	return new LitOpd(std::to_string(myNum), QUADWORD);
}

Opd * StrLitNode::flatten(Procedure * proc){
	Opd * res = proc->getProg()->makeString(myStr);
	return res;
}

Opd * CharLitNode::flatten(Procedure * proc){
	return new LitOpd(std::string(1, myVal), BYTE);
}

Opd * FalseNode::flatten(Procedure * prog){
	Opd * res = new LitOpd("0", BYTE);
	return res;
}

Opd * TrueNode::flatten(Procedure * prog){
	Opd * res = new LitOpd("1", BYTE);
	return res;
}

Opd * AssignExpNode::flatten(Procedure * proc){
	Opd * rhs = mySrc->flatten(proc);
	Opd * lhs = myDst->flatten(proc);
	if (!lhs){
		throw InternalError("null tgt");
	}
	
	AssignQuad * quad = new AssignQuad(lhs, rhs);
	quad->setComment("Assign");
	proc->addQuad(quad);
	return lhs;
}

static void argsTo3AC(Procedure * proc, std::list<ExpNode *> * args){
	std::list<Opd *> argOpds;
	for (auto elt : *args){
		Opd * argOpd = elt->flatten(proc);
		argOpds.push_back(argOpd);
	}
	size_t argIdx = 1;
	for (auto argOpd : argOpds){
		Quad * argQuad = new SetArgQuad(argIdx, argOpd);
		proc->addQuad(argQuad);
		argIdx++;
	}
}

Opd * CallExpNode::flatten(Procedure * proc){
	argsTo3AC(proc, myArgs);
	Quad * callQuad = new CallQuad(myID->getSymbol());
	proc->addQuad(callQuad);

	SemSymbol * idSym = myID->getSymbol();
	const FnType * calleeType = idSym->getDataType()->asFn();
	const DataType * retType = calleeType->getReturnType();
	if (retType->isVoid()){
		return nullptr;
	} else {
		Opd * retVal = proc->makeTmp(Opd::width(retType));
		Quad * getRet = new GetRetQuad(retVal);
		proc->addQuad(getRet);
		return retVal;
	}
}

Opd * NegNode::flatten(Procedure * proc){
	Opd * child = myExp->flatten(proc);
	OpdWidth width = QUADWORD;
	Opd * dst = proc->makeTmp(width);
	Quad * quad = new UnaryOpQuad(dst, NEG, child);
	proc->addQuad(quad);
	return dst;
}

Opd * NotNode::flatten(Procedure * proc){
	Opd * child = myExp->flatten(proc);
	OpdWidth width = BYTE;
	Opd * dst = proc->makeTmp(width);
	Quad * quad = new UnaryOpQuad(dst, NOT, child);
	proc->addQuad(quad);
	return dst;
}

Opd * PlusNode::flatten(Procedure * proc){
	Opd * childL = myExp1->flatten(proc);
	Opd * childR = myExp2->flatten(proc);
	OpdWidth width = QUADWORD;
	Opd * dst = proc->makeTmp(width);
	Quad * quad = new BinOpQuad(dst, ADD, childL, childR);
	proc->addQuad(quad);
	return dst;
}

Opd * MinusNode::flatten(Procedure * proc){
	Opd * childL = myExp1->flatten(proc);
	Opd * childR = myExp2->flatten(proc);
	OpdWidth width = QUADWORD;
	Opd * dst = proc->makeTmp(width);
	Quad * quad = new BinOpQuad(dst, SUB, childL, childR);
	proc->addQuad(quad);
	return dst;
}

Opd * TimesNode::flatten(Procedure * proc){
	Opd * childL = myExp1->flatten(proc);
	Opd * childR = myExp2->flatten(proc);
	OpdWidth width = QUADWORD;
	Opd * dst = proc->makeTmp(width);
	Quad * quad = new BinOpQuad(dst, MULT, childL, childR);
	proc->addQuad(quad);
	return dst;
}

Opd * DivideNode::flatten(Procedure * proc){
	Opd * op1 = this->myExp1->flatten(proc);
	Opd * op2 = this->myExp2->flatten(proc);
	OpdWidth width = QUADWORD;
	Opd * opRes = proc->makeTmp(width);
	BinOpQuad * quad = new BinOpQuad(opRes, DIV, op1, op2);
	proc->addQuad(quad);
	return opRes;
}

Opd * AndNode::flatten(Procedure * proc){
	Opd * op1 = this->myExp1->flatten(proc);
	Opd * op2 = this->myExp2->flatten(proc);
	OpdWidth width = BYTE;
	Opd * opRes = proc->makeTmp(width);
	BinOpQuad * quad = new BinOpQuad(opRes, AND, op1, op2);
	proc->addQuad(quad);
	return opRes;
}

Opd * OrNode::flatten(Procedure * proc){
	Opd * op1 = this->myExp1->flatten(proc);
	Opd * op2 = this->myExp2->flatten(proc);
	OpdWidth width = BYTE;
	Opd * opRes = proc->makeTmp(width);
	BinOpQuad * quad = new BinOpQuad(opRes, OR, op1, op2);
	proc->addQuad(quad);
	return opRes;
}

Opd * EqualsNode::flatten(Procedure * proc){
	Opd * op1 = this->myExp1->flatten(proc);
	Opd * op2 = this->myExp2->flatten(proc);
	OpdWidth width = proc->getProg()->opWidth(this);
	Opd * opRes = proc->makeTmp(width);
	BinOpQuad * quad = new BinOpQuad(opRes, EQ, op1, op2);
	proc->addQuad(quad);
	return opRes;
}

Opd * NotEqualsNode::flatten(Procedure * proc){
	Opd * op1 = this->myExp1->flatten(proc);
	Opd * op2 = this->myExp2->flatten(proc);
	OpdWidth width = proc->getProg()->opWidth(this);
	Opd * opRes = proc->makeTmp(width);
	BinOpQuad * quad = new BinOpQuad(opRes, NEQ, op1, op2);
	proc->addQuad(quad);
	return opRes;
}

Opd * GreaterNode::flatten(Procedure * proc){
	Opd * op1 = this->myExp1->flatten(proc);
	Opd * op2 = this->myExp2->flatten(proc);
	OpdWidth width = proc->getProg()->opWidth(this);
	Opd * opRes = proc->makeTmp(width);
	BinOpQuad * quad = new BinOpQuad(opRes, GT, op1, op2);
	proc->addQuad(quad);
	return opRes;
}

Opd * GreaterEqNode::flatten(Procedure * proc){
	Opd * op1 = this->myExp1->flatten(proc);
	Opd * op2 = this->myExp2->flatten(proc);
	OpdWidth width = proc->getProg()->opWidth(this);
	Opd * opRes = proc->makeTmp(width);
	BinOpQuad * quad = new BinOpQuad(opRes, GTE, op1, op2);
	proc->addQuad(quad);
	return opRes;
}

Opd * LessNode::flatten(Procedure * proc){
	Opd * op1 = this->myExp1->flatten(proc);
	Opd * op2 = this->myExp2->flatten(proc);
	OpdWidth width = proc->getProg()->opWidth(this);
	Opd * opRes = proc->makeTmp(width);
	BinOpQuad * quad = new BinOpQuad(opRes, LT, op1, op2);
	proc->addQuad(quad);
	return opRes;
}

Opd * LessEqNode::flatten(Procedure * proc){
	Opd * op1 = this->myExp1->flatten(proc);
	Opd * op2 = this->myExp2->flatten(proc);
	OpdWidth width = proc->getProg()->opWidth(this);
	Opd * opRes = proc->makeTmp(width);
	BinOpQuad * quad = new BinOpQuad(opRes, LTE, op1, op2);
	proc->addQuad(quad);
	return opRes;
}

void AssignStmtNode::to3AC(Procedure * proc){
	Opd * res = myExp->flatten(proc);
	// Since we're at the stmt level, we know
	// this opd isn't going to be used. We 
	// could delete it
}

void PostIncStmtNode::to3AC(Procedure * proc){
	Opd * child = this->myLVal->flatten(proc);
	OpdWidth width = QUADWORD;
	LitOpd * litOpd = new LitOpd("1", width);
	BinOpQuad * quad = new BinOpQuad(child, ADD, child, litOpd);
	proc->addQuad(quad);
}

void PostDecStmtNode::to3AC(Procedure * proc){
	Opd * child = this->myLVal->flatten(proc);
	OpdWidth width = QUADWORD;
	LitOpd * litOpd = new LitOpd("1", width);
	BinOpQuad * quad = new BinOpQuad(child, SUB, child, litOpd);
	proc->addQuad(quad);
}

void FromConsoleStmtNode::to3AC(Procedure * proc){
	Opd * child = this->myDst->flatten(proc);
	IntrinsicQuad * quad = new IntrinsicQuad(INPUT, child);
	proc->addQuad(quad);
}

void ToConsoleStmtNode::to3AC(Procedure * proc){
	Opd * child = this->mySrc->flatten(proc);
	IntrinsicQuad * quad = new IntrinsicQuad(OUTPUT, child);
	proc->addQuad(quad);
}

void IfStmtNode::to3AC(Procedure * proc){
	Opd * cond = myCond->flatten(proc);
	Label * afterLabel = proc->makeLabel();
	Quad * afterNop = new NopQuad();
	afterNop->addLabel(afterLabel);

	proc->addQuad(new JmpIfQuad(cond, afterLabel));
	for (auto stmt : *myBody){
		stmt->to3AC(proc);
	}
	proc->addQuad(afterNop);
}

void IfElseStmtNode::to3AC(Procedure * proc){
	Label * elseLabel = proc->makeLabel();
	Quad * elseNop = new NopQuad();
	elseNop->addLabel(elseLabel);
	Label * afterLabel = proc->makeLabel();
	Quad * afterNop = new NopQuad();
	afterNop->addLabel(afterLabel);

	Opd * cond = myCond->flatten(proc);

	Quad * jmpFalse = new JmpIfQuad(cond, elseLabel);
	proc->addQuad(jmpFalse);
	for (auto stmt : *myBodyTrue){
		stmt->to3AC(proc);
	}
	
	Quad * skipFall = new JmpQuad(afterLabel);
	proc->addQuad(skipFall);

	proc->addQuad(elseNop);
	
	for (auto stmt : *myBodyFalse){
		stmt->to3AC(proc);
	}

	proc->addQuad(afterNop);
}

void WhileStmtNode::to3AC(Procedure * proc){
	Quad * headNop = new NopQuad();
	Label * headLabel = proc->makeLabel();
	headNop->addLabel(headLabel);

	Label * afterLabel = proc->makeLabel();
	Quad * afterQuad = new NopQuad();
	afterQuad->addLabel(afterLabel);

	proc->addQuad(headNop);
	Opd * cond = myCond->flatten(proc);
	Quad * jmpFalse = new JmpIfQuad(cond, afterLabel);
	proc->addQuad(jmpFalse);

	for (auto stmt : *myBody){
		stmt->to3AC(proc);
	}

	Quad * loopBack = new JmpQuad(headLabel);
	proc->addQuad(loopBack);
	proc->addQuad(afterQuad);
}

void CallStmtNode::to3AC(Procedure * proc){
	Opd * res = myCallExp->flatten(proc);
	//Since we're in a callStmt, the GetRet quad
	// generated as the last action of the subtree
	// was unnecessary. Remove it from the procedure.
	if (res != nullptr){
		//A void call will not generate a getout
		Quad * last = proc->popQuad();
	}
	//Should probably delete the last quad, but
	// we've leaked so much memory why start worrying now?
}

void ReturnStmtNode::to3AC(Procedure * proc){
	if (myExp != nullptr){
		Opd * res = myExp->flatten(proc);
		Quad * setOut = new SetRetQuad(res);
		proc->addQuad(setOut);
	}
	
	Label * leaveLbl = proc->getLeaveLabel();
	Quad * jmpLeave = new JmpQuad(leaveLbl);
	proc->addQuad(jmpLeave);
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

Opd * NullPtrNode::flatten(Procedure * prog){
	Opd * res = new LitOpd("0", ADDR);
	return res;
}

Opd * RefNode::flatten(Procedure * proc){
	Opd * dst = proc->makeTmp(ADDR);
	Opd * child = myID->flatten(proc);
	LocQuad * loc = new LocQuad(child, dst, true);
	
	proc->addQuad(loc);
	return dst;
}

Opd * DerefNode::flatten(Procedure * proc){
	Opd * dst = proc->makeTmp(ADDR);
	Opd * getVal = myID->flatten(proc);
	LocQuad * mem = new LocQuad(getVal, dst, false);

	proc->addQuad(mem);
	return dst;
}

Opd * IndexNode::flatten(Procedure * proc){
	Opd * baseOpd = myBase->flatten(proc);
	Opd * offOpd = myOffset->flatten(proc);
	Opd * strideOpd = nullptr;
	OpdWidth width = baseOpd->getWidth();
	if (width == ADDR){
		LitOpd * litOpd = new LitOpd("4", ADDR);
		strideOpd = proc->makeTmp(ADDR);
		Quad * strideQuad = new BinOpQuad(strideOpd, MULT, offOpd, litOpd);
	} else if (width == QUADWORD){
		LitOpd * litOpd = new LitOpd("4", ADDR);
		strideOpd = proc->makeTmp(ADDR);
		Quad * strideQuad = new BinOpQuad(strideOpd, MULT, offOpd, litOpd);
	} else if (width == BYTE){
		strideOpd = offOpd;
	}
	
	Opd * tmp1 = proc->makeTmp(ADDR);
	Quad * offQuad = new BinOpQuad(tmp1, ADD, baseOpd, strideOpd);
	proc->addQuad(offQuad);

	Opd * tmp2 = proc->makeTmp(ADDR);
	LocQuad * baseQuad = new LocQuad(tmp1, tmp2, false);
	proc->addQuad(baseQuad);

	return tmp2;
}

}
