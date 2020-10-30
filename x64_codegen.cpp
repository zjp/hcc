#include <ostream>
#include "3ac.hpp"
#include "err.hpp"

namespace holeyc{

void IRProgram::allocGlobals(){
	// Give a label to everything
	TODO(Implement me)
}

void IRProgram::datagenX64(std::ostream& out){
	TODO(Write out data section)
	out << ".text\n";
	 for(auto global : globals) {
		switch(type) {
			case a:
				out << ".quad" << "\n";
				break;
	 		case b:
	 			out << ".asciiz" << "\n";
	 			break;
	 		default: break; 
		}
	}
	//Put this directive after you write out strings
	// so that everything is aligned to a quadword value
	// again
	out << ".align 8\n";
}

void IRProgram::toX64(std::ostream& out){
	TODO(Implement me)
}

void Procedure::allocLocals(){
	TODO(Implement me)
}

void Procedure::toX64(std::ostream& out){
	//Allocate all locals
	allocLocals();

	enter->codegenLabels(out);
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

	size_t numLabels = labels.size();
	size_t labelIdx = 0;
	for ( Label * label : labels){
		out << label->toString() << ": ";
		if (labelIdx != numLabels - 1){ out << "\n"; }
		labelIdx++;
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
		case OR:
			opString = "or";
			break;
		case AND:
			opString = "and"
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
	// TODO(Implement me)
}

void JmpQuad::codegenX64(std::ostream& out){
	out << "jmp " << tgt->toString() << "\n";
}

void JmpIfQuad::codegenX64(std::ostream& out){
	TODO(Implement me)
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
		// myArg->genStore(out, "register");
		TODO("IMPLEMENT ME");
	}
}

void CallQuad::codegenX64(std::ostream& out){
	out << "call" << "\n";
	TODO(Implement me)
}

void EnterQuad::codegenX64(std::ostream& out){
	// out << 
	TODO(Implement me)
}

void LeaveQuad::codegenX64(std::ostream& out){
	out << "leave " << "\n";
	TODO(Implement me)
}

void SetArgQuad::codegenX64(std::ostream& out){
	// out << setarg <<
	TODO(Implement me)
}

void GetArgQuad::codegenX64(std::ostream& out){
	//We don't actually need to do anything here
}

void SetRetQuad::codegenX64(std::ostream& out){
	TODO(Implement me)
}

void GetRetQuad::codegenX64(std::ostream& out){
	TODO(Implement me)
}

void SymOpd::genLoad(std::ostream & out, std::string regStr){
	TODO(Implement me)
}

void SymOpd::genStore(std::ostream& out, std::string regStr){
	TODO(Implement me)
}

void AuxOpd::genLoad(std::ostream & out, std::string regStr){
	TODO(Implement me)
}

void AuxOpd::genStore(std::ostream& out, std::string regStr){

	TODO(Implement me)
}

void LitOpd::genLoad(std::ostream & out, std::string regStr){
	TODO(Implement me)
}

void LitOpd::genStore(std::ostream& out, std::string regStr){
	throw new InternalError("Cannot use literal as l-val");
}

}
