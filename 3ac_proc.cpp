#include "3ac.hpp"

namespace holeyc{

Procedure::Procedure(IRProgram * prog, std::string name)
: myProg(prog), myName(name){
	maxTmp = 0;
	enter = new EnterQuad(this);
	leave = new LeaveQuad(this);
	if (myName.compare("main") == 0){
		enter->addLabel(new Label("main"));
	} else {
		enter->addLabel(new Label("fun_" + myName));
	}
	leaveLabel = myProg->makeLabel();
	leave->addLabel(leaveLabel);
}

std::string Procedure::getName(){
	return myName;
}

Label * Procedure::getLeaveLabel(){
	return leaveLabel;
}

IRProgram * Procedure::getProg(){ return myProg; }

std::string Procedure::toString(bool verbose){
	std::string res = "";

	res += "[BEGIN " + this->getName() + " LOCALS]\n";
	for (const auto formal : this->formals){
		res += formal->toString() + " (formal)\n";
	}

	for (auto local : this->locals){
		res += local.second->toString() + " (local)\n";
	}

	for (auto tmp : temps){
		res += tmp->toString() + " (tmp)\n";
	}
	res += "[END " + this->getName() + " LOCALS]\n";

	res += enter->toString(verbose) + "\n";
	for (auto quad : bodyQuads){
		res += quad->toString(verbose) + "\n";
	}
	res += leave->toString(verbose) + "\n";
	return res;
}

Label * Procedure::makeLabel(){
	return myProg->makeLabel();
}

void Procedure::addQuad(Quad * quad){
	bodyQuads.push_back(quad);
}

Quad * Procedure::popQuad(){
	Quad * last = bodyQuads.back();
	bodyQuads.pop_back();
	return last;
}

void Procedure::gatherLocal(SemSymbol * sym){
	OpdWidth width = Opd::width(sym->getDataType());
	locals[sym] = new SymOpd(sym, width);
}

void Procedure::gatherFormal(SemSymbol * sym){
	OpdWidth width = Opd::width(sym->getDataType());
	formals.push_back(new SymOpd(sym, width));
}

SymOpd * Procedure::getSymOpd(SemSymbol * sym){
	for(auto formalSeek : formals){
		if (formalSeek->getSym() == sym){
			return formalSeek;
		}
	}

	auto localFound = locals.find(sym);
	if (localFound != locals.end()){
		return localFound->second;
	}
	
	return this->getProg()->getGlobal(sym);
}

AuxOpd * Procedure::makeTmp(OpdWidth width){
	std::string name = "tmp";
	name += std::to_string(maxTmp++);
	AuxOpd * res = new AuxOpd(name, width);
	temps.push_back(res);

	return res;
}

}
