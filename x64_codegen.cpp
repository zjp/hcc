#include <ostream>
#include "3ac.hpp"
#include "err.hpp"

namespace holeyc{

static void codegen_indent(std::ostream& out) {
	for(int i = 0; i < 4; i++) {
		out << " ";
	}
}

void IRProgram::allocGlobals() {
	for(auto global : globals) {
	// Give a label to everything
		global.second->setMemoryLoc("gbl_" + global.first->getName());
	}
}

void IRProgram::datagenX64(std::ostream& out){
	out << ".data\n";
	 for(auto global : globals) {
		 codegen_indent(out);
		 out << global.second->getMemoryLoc() << ": .quad\n";
	}
	//Put this directive after you write out strings
	// so that everything is aligned to a quadword value
	// again
	out << ".align 8\n";
	out << ".text\n";
	for(auto proc : procs) {
		out << ".globl " << proc->getName() << "\n";
	}
}

void IRProgram::toX64(std::ostream& out){
	allocGlobals();
	datagenX64(out);
	for(auto proc : procs) {
		proc->toX64(out);
	}
}

void Procedure::allocLocals(){
	int i = 1;
	int offset = 0;
	std::string loc = "(%rbp)";
	for(auto local : locals) {
		std::string offset = std::to_string(8*i);
		local.second->setMemoryLoc(offset + loc);
		++i;
	}
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
	std::string regStringSrc1;
	std::string regStringSrc2;
	switch(op) {
		case ADD:
			regStringSrc1 = "%rax";
			regStringSrc1 = "%rbx";
			opString = "addq ";
			break;
		case SUB:
			regStringSrc1 = "%rax";
			regStringSrc1 = "%rbx";
			opString = "subq ";
			break;
		case MULT:
			regStringSrc1 = "%rax";
			regStringSrc1 = "%rbx";
			opString = "imulq ";
			break;
		case DIV:
			regStringSrc1 = "%rax";
			regStringSrc1 = "%rbx";
			opString = "idivq ";
			break;
		case EQ:
			regStringSrc1 = "%rax";
			regStringSrc1 = "%rbx";
			opString = "cmpq ";
			break;
		case NEQ:
			regStringSrc1 = "%rax";
			regStringSrc1 = "%rbx";
			opString = "cmpq ";
			break;
		case GTE:
			regStringSrc1 = "%rax";
			regStringSrc1 = "%rbx";
			opString = "cmpq ";
			break;
		case LTE:
			regStringSrc1 = "%rax";
			regStringSrc1 = "%rbx";
			opString = "cmpq ";
			break;
		case LT:
			regStringSrc1 = "%rax";
			regStringSrc1 = "%rbx";
			opString = "cmpq ";
			break;
		case GT:
			regStringSrc1 = "%rax";
			regStringSrc1 = "%rbx";
			opString = "cmpq ";
			break;
		case OR:
			regStringSrc1 = "%rax";
			regStringSrc1 = "%rbx";
			opString = "orq ";
			break;
		case AND:
			regStringSrc1 = "%rax";
			regStringSrc1 = "%rbx";
			opString = "andq ";
			break;
		default: break;
	}
	src1->genLoad(out, regStringSrc1);
	src2->genLoad(out, regStringSrc2);
	codegen_indent(out);
	out << opString << "%rax, %rbx\n";
	/* clean up with setl if necessary */
	switch(op) {
		case EQ:
			codegen_indent(out);
			out << "sete" << "\n";
			break;
		case NEQ:
			codegen_indent(out);
			out << "sete" << "\n";
			break;
		case GTE:
			codegen_indent(out);
			out << "setge" << "\n";
			break;
		case LTE:
			codegen_indent(out);
			out << "setle" << "\n";
			break;
		case LT:
			codegen_indent(out);
			out << "setl" << "\n";
			break;
		case GT:
			codegen_indent(out);
			out << "setg" << "\n";
			break;
		default: break;
	}
}

void UnaryOpQuad::codegenX64(std::ostream& out){
	std::string opString;
	std::string regString;
	switch(op) {
		case NEG:
			regString = "%rax";

			opString = "negq ";
			break;
		case NOT:
			regString = "%rcx";
			opString = "notq ";
	}
	src->genLoad(out, regString);
	out << "movq " << /* opd << reg << */"\n";
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
	codegen_indent(out);
	out << "jmp " << tgt->toString() << "\n";
}

void JmpIfQuad::codegenX64(std::ostream& out){
	codegen_indent(out);
	out << "movq $0, %rax" << "\n";
	codegen_indent(out);
	out << "movb" << "\n";
	codegen_indent(out);
	out << "cmpq $0 %rax\n";
	codegen_indent(out);
	out << "je " << tgt->toString() << "\n";
}

void NopQuad::codegenX64(std::ostream& out){
	codegen_indent(out);
	out << "nop" << "\n";
}

void IntrinsicQuad::codegenX64(std::ostream& out){
	switch(myIntrinsic){
	case OUTPUT:
		myArg->genLoad(out, "%rdi");
		codegen_indent(out);
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
	codegen_indent(out);
	out << "callq " << callee->getName() << "\n";
}

void EnterQuad::codegenX64(std::ostream& out){
	int tempLocalSize = myProc->localsSize();
	if(tempLocalSize % 16) {
		tempLocalSize += 8;
	}
	codegen_indent(out);
	out << "pushq %rbp" << "\n";
	codegen_indent(out);
	out << "movq %rsp, %rbp" << "\n";
	codegen_indent(out);
	out << "addq $16, %rbp" << "\n";
	codegen_indent(out);
	out << "subq $" << tempLocalSize << ", %rsp" << "\n";
}

void LeaveQuad::codegenX64(std::ostream& out){
	int tempLocalSize = myProc->localsSize();
	if(tempLocalSize % 16) {
		tempLocalSize += 8;
	}
	codegen_indent(out);
	out << "addq $" << tempLocalSize <<  ", %rsp" << "\n";
	codegen_indent(out);
	out << "popq " << "%rbp" << "\n";
	codegen_indent(out);
	out << "retq\n";
}

void SetArgQuad::codegenX64(std::ostream& out){
	codegen_indent(out);
	out << "setarg" << "\n";
}

void GetArgQuad::codegenX64(std::ostream& out){
	//We don't actually need to do anything here
	/* OR: Move values from each register to the activation record */
}

void SetRetQuad::codegenX64(std::ostream& out){
	codegen_indent(out);
	out << "setret" << "\n";
}

void GetRetQuad::codegenX64(std::ostream& out){
	codegen_indent(out);
	out << "getret" << "\n";
}

void SymOpd::genLoad(std::ostream & out, std::string regStr){
	codegen_indent(out);
	out << "movq " << regStr << ", " << "\n";
}

void SymOpd::genStore(std::ostream& out, std::string regStr){
	codegen_indent(out);
	out << regStr << "\n";
}

void AuxOpd::genLoad(std::ostream & out, std::string regStr){
	codegen_indent(out);
	out << "movq " << regStr << "\n";
}

void AuxOpd::genStore(std::ostream& out, std::string regStr){
	codegen_indent(out);
	out << "movq " << regStr << " " << "\n";
}

void LitOpd::genLoad(std::ostream & out, std::string regStr){
	// e.g. movq eax, val
	codegen_indent(out);
	out << "movq $" << val << ", " << regStr << "\n";
}

void LitOpd::genStore(std::ostream& out, std::string regStr){
	throw new InternalError("Cannot use literal as l-val");
}

}
