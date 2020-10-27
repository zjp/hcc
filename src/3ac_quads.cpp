#include "3ac.hpp"

namespace holeyc{


Quad::Quad() : myComment(""){
}

void Quad::addLabel(Label * label){
	labels.push_back(label);
}

void Quad::setComment(std::string commentIn){
	this->myComment = commentIn;
}

std::string Quad::commentStr(){
	if (myComment.length() > 0){
		return "  #" + myComment;
	}
	return "";
}

std::string Quad::toString(bool verbose){
	auto res = std::string("");

	auto first = true;
	for (auto label : labels){
		if (first){ first = false; }
		else { res += ","; }

		res += label->toString();
	}
	if (!first){ res += ": "; }

	res += this->repr();
	if (verbose){
		res += commentStr();
	}

	return res;
}

CallQuad::CallQuad(SemSymbol * calleeIn) : callee(calleeIn){ }

std::string CallQuad::repr(){
	return "call " + callee->getName();
}

EnterQuad::EnterQuad(Procedure * procIn) : Quad(), myProc(procIn)
{ }

std::string EnterQuad::repr(){
	return "enter " + myProc->getName();
}

LeaveQuad::LeaveQuad(Procedure * procIn) : Quad(), myProc(procIn)
{ }

std::string LeaveQuad::repr(){
	return "leave " + myProc->getName();
}

std::string AssignQuad::repr(){
	return dst->toString() + " := " + src->toString();
	
}

BinOpQuad::BinOpQuad(Opd * dstIn, BinOp opIn, Opd * src1In, Opd * src2In)
: dst(dstIn), op(opIn), src1(src1In), src2(src2In){ }

std::string BinOpQuad::repr(){
	std::string opString;
	switch (op){
	case ADD:
		opString = " ADD64 ";
		break;
	case SUB:
		opString = " SUB64 ";
		break;
	case DIV:
		opString = " DIV64 ";
		break;
	case MULT:
		opString = " MULT64 ";
		break;
	case OR:
		opString = " OR8 ";
		break;
	case AND:
		opString = " AND8 ";
		break;
	case EQ:
		if (dst->getWidth() == BYTE){
			opString = " EQ8 ";
		} else {
			opString = " EQ64 ";
		}
		break;
	case NEQ:
		if (dst->getWidth() == BYTE){
			opString = " NEQ8 ";
		} else {
			opString = " NEQ64 ";
		}
		break;
	case LT:
		opString = " LT64 ";
		break;
	case GT:
		opString = " GT64 ";
		break;
	case LTE:
		opString = " LTE64 ";
		break;
	case GTE:
		opString = " GTE64 ";
		break;
	}
	return dst->toString() + " := " 
		+ src1->toString()
		+ opString
		+ src2->toString();
}

UnaryOpQuad::UnaryOpQuad(Opd * dstIn, UnaryOp opIn, Opd * srcIn)
: dst(dstIn), op(opIn), src(srcIn) { }

std::string UnaryOpQuad::repr(){
	std::string opString;
	switch (op){
	case NEG:
		opString = "NEG64 ";
		break;
	case NOT:
		opString = "NOT8 ";
	}
	return dst->toString() + " := " 
		+ opString
		+ src->toString();
}

IntrinsicQuad::IntrinsicQuad(Intrinsic intrinsic, Opd * opd) 
: myArg(opd), myIntrinsic(intrinsic){ }

std::string IntrinsicQuad::repr(){
	std::string res;
	switch (myIntrinsic){
	case INPUT:
		res = "INPUT " + myArg->toString();
		break;
	case OUTPUT:
		res = "OUTPUT " + myArg->toString();
		break;
	}
	return res;
}

JmpQuad::JmpQuad(Label * tgtIn)
: Quad(), tgt(tgtIn){ }

std::string JmpQuad::repr(){
	std::string res = "";
	return "goto " + tgt->toString();
}

JmpIfQuad::JmpIfQuad(Opd * cndIn, Label * tgtIn) 
: Quad(), cnd(cndIn), tgt(tgtIn){ }

std::string JmpIfQuad::repr(){
	std::string res = "IFZ ";
	res += cnd->toString();
	res += " GOTO ";
	res += tgt->toString();
	return res;
}

NopQuad::NopQuad()
: Quad() { }

std::string NopQuad::repr(){
	return "nop";
}

GetRetQuad::GetRetQuad(Opd * opdIn)
: Quad(), opd(opdIn) { }

std::string GetRetQuad::repr(){
	std::string res = "";
	res += "getret " + opd->toString(); 
	return res;
}

SetArgQuad::SetArgQuad(size_t indexIn, Opd * opdIn) 
: index(indexIn), opd(opdIn){
}

std::string SetArgQuad::repr(){
	std::string res = "";
	res += "setarg " + std::to_string(index) + " " + opd->toString(); 
	return res;
}

GetArgQuad::GetArgQuad(size_t indexIn, Opd * opdIn) 
: index(indexIn), opd(opdIn){
}

std::string GetArgQuad::repr(){
	std::string res = "";
	res += "getarg " + std::to_string(index) + " " + opd->toString(); 
	return res;
}

SetRetQuad::SetRetQuad(Opd * opdIn) 
: opd(opdIn){
}

std::string SetRetQuad::repr(){
	std::string res = "";
	res += "setret " + std::to_string(index) + " " + opd->toString(); 
	return res;
}

}


