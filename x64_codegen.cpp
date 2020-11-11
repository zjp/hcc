#include <ostream>
#include "3ac.hpp"
#include "err.hpp"

namespace holeyc{

void IRProgram::allocGlobals(){
	// Give a label to everything
}

void IRProgram::datagenX64(std::ostream& out){
	out << ".data\n";
	 for(auto global : globals) {
		 out << "gbl_" << global.first->getName() << " .quad" << "\n";
		 out << "gbl_" << global.first->getName() << " .asciiz" << "\n";
	}
	//Put this directive after you write out strings
	// so that everything is aligned to a quadword value
	// again
	out << ".align 8\n";
	out << ".text\n";
}

void IRProgram::toX64(std::ostream& out){
	allocGlobals();
	datagenX64(out);
	for(auto proc : procs) {
		proc->toX64(out);
	}
}

void Procedure::allocLocals(){

}

void Procedure::toX64(std::ostream& out){
	//Allocate all locals
	allocLocals();

	if(myName == "main") {
		out << "main:\n";
	} else {
		enter->codegenLabels(out);
        }
        enter->codegenX64(out);
	for (auto quad : *bodyQuads){
		quad->codegenLabels(out);
		quad->codegenX64(out);
	}
	leave->codegenLabels(out);
	leave->codegenX64(out);
}

void Quad::codegenLabels(std::ostream& out){
	if (labels.empty()){ return; }
	for ( Label * label : labels){
		out << label->toString() << ":\n";
	}
}

void BinOpQuad::codegenX64(std::ostream& out){
	std::string opString;
	switch(op) {
		case ADD:
			opString = "add";
			break;
		case SUB:
			opString = "sub";
			break;
		case MULT:
			opString = "imul";
			break;
		case DIV:
			opString = "idivq";
			break;
		case EQ:
			opString = "cmp";
			break;
		case NEQ:
			opString = "cmp";
			break;
		case GTE:
			opString = "cmp";
			break;
		case LTE:
			opString = "cmp";
			break;
		case LT:
			opString = "cmp";
			break;
		case GT:
			opString = "cmp";
			break;
		case OR:
			opString = "or";
			break;
		case AND:
			opString = "and";
			break;
	}
	out << opString << "\n";
}

void UnaryOpQuad::codegenX64(std::ostream& out){
	std::string opString;
	switch(op) {
		case NEG:
			opString = "neg ";
			break;
		case NOT:
			opString = "not ";
	}
	out << opString << "\n";
}

void AssignQuad::codegenX64(std::ostream& out){
	src->genLoad(out, "%rax");
	dst->genStore(out, "%rax");
}

void LocQuad::codegenX64(std::ostream& out){
	// (Optional)
}

void JmpQuad::codegenX64(std::ostream& out){
	out << "jmp " << tgt->toString() << "\n";
}

void JmpIfQuad::codegenX64(std::ostream& out){
}

void NopQuad::codegenX64(std::ostream& out){
	out << "nop" << "\n";
}

void IntrinsicQuad::codegenX64(std::ostream& out){
	switch(myIntrinsic){
	case OUTPUT:
		myArg->genLoad(out, "%rdi");
		if (myArg->getWidth() == QUADWORD){
			out << "callq printInt\n";
		} else if (myArg->getWidth() == BYTE){
			out << "callq printByte\n";
		} else {
			//If the argument is an ADDR,
			// assume it's a string
			out << "callq printString";
		}
		break;
	case INPUT:
		 myArg->genStore(out, "register");
	}
}

void CallQuad::codegenX64(std::ostream& out){
	out << "callq " << callee->getName() << "\n";
}

void EnterQuad::codegenX64(std::ostream& out){
	out << "enter " << "\n";
}

void LeaveQuad::codegenX64(std::ostream& out){
	out << "leave " << "\n";
}

void SetArgQuad::codegenX64(std::ostream& out){
	out << "setarg" << "\n";
}

void GetArgQuad::codegenX64(std::ostream& out){
	//We don't actually need to do anything here
}

void SetRetQuad::codegenX64(std::ostream& out){
	out << "setret" << "\n";
}

void GetRetQuad::codegenX64(std::ostream& out){
	out << "getret" << "\n";
}

void SymOpd::genLoad(std::ostream & out, std::string regStr){
	out << regStr << "\n";
}

void SymOpd::genStore(std::ostream& out, std::string regStr){
	out << regStr << "\n";
}

void AuxOpd::genLoad(std::ostream & out, std::string regStr){
	out << regStr << "\n";
}

void AuxOpd::genStore(std::ostream& out, std::string regStr){
	out << regStr << "\n";
}

void LitOpd::genLoad(std::ostream & out, std::string regStr){
	// e.g. movq eax, val
	out << "movq" << regStr << ", " << val << "\n";
}

void LitOpd::genStore(std::ostream& out, std::string regStr){
	throw new InternalError("Cannot use literal as l-val");
}

}
