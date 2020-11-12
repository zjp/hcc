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
	out << ".data\n\n";
	for(auto global : globals) {
		codegen_indent(out);
		out << global.second->getMemoryLoc() << ": .quad 0\n";
	}
	//Put this directive after you write out strings
	// so that everything is aligned to a quadword value
	// again
	out << "\n.align 8\n";
	out << "\n.text\n";
	out << ".globl main\n\n";
}

void IRProgram::toX64(std::ostream& out){
	allocGlobals();
	datagenX64(out);
	for(auto proc : procs) {
		proc->toX64(out);
	}
}

void Procedure::allocLocals(){
	int i = 3;
	std::string loc = "(%rbp)";
	for(auto local : locals) {
		std::string offset = std::to_string(-8*i);
		local.second->setMemoryLoc(offset + loc);
		++i;
	}
	for(auto formal : formals) {
		std::string offset = std::to_string(-8*i);
		formal->setMemoryLoc(offset + loc);
		++i;
	}
        i = 8;
        /* I'll be honest here, I have no idea why this works or even if it does */
	for(auto tmp : temps) {
		tmp->setMemoryLoc("%r" + std::to_string(i));
		++i;
		if(i == 15) {
			i = 8;
		}
	}
}

void Procedure::toX64(std::ostream& out){
	//Allocate all locals
	allocLocals();

	out << this->getName() << ":\n"; // "main:\n";
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
	std::string regStringSrc1 = "%rax";
	std::string regStringSrc2 = "%rbx";
	std::string regDstString = "%rbx";
	switch(op) {
		case ADD:
			opString = "addq";
			break;
		case SUB:
			opString = "subq";
			break;
		case MULT:
			codegen_indent(out);
			out << "movq $0, %rdx\n";
			opString = "imulq";
			break;
		case DIV:
			codegen_indent(out);
			out << "movq $0, %rdx\n";
			opString = "idivq";
			break;
		case EQ:
			opString = "cmpq";
			break;
		case NEQ:
			opString = "cmpq";
			break;
		case GTE:
			opString = "cmpq";
			break;
		case LTE:
			opString = "cmpq";
			break;
		case LT:
			opString = "cmpq";
			break;
		case GT:
			opString = "cmpq";
			break;
		case OR:
			opString = "orq";
			break;
		case AND:
			opString = "andq";
			break;
		default: break;
	}
	src1->genLoad(out, regStringSrc1);
	src2->genLoad(out, regStringSrc2);
	codegen_indent(out);
	switch (op) {
		case ADD:
			out << opString << " " << regStringSrc1 << ", "
			    << regStringSrc2 << "\n";
			break;
		case SUB:
			out << opString << " " << regStringSrc1 << ", "
			    << regStringSrc2 << "\n";
			break;
		case DIV:
			out << opString << " " << regStringSrc2 << "\n";
			codegen_indent(out);
			out << "movq %rax, %rbx\n";
			break;
		case MULT:
			out << opString << " " << regStringSrc2 << "\n";
			codegen_indent(out);
			out << "movq %rax, %rbx\n";
			break;
		case EQ:
			out << opString << " " << regStringSrc2 << ", "
			    << regStringSrc1 << "\n";
			break;
		case NEQ:
			out << opString << " " << regStringSrc2 << ", "
			    << regStringSrc1 << "\n";
			break;
		case GTE:
			out << opString << " " << regStringSrc2 << ", "
			    << regStringSrc1 << "\n";
			break;
		case LTE:
			out << opString << " " << regStringSrc2 << ", "
			    << regStringSrc1 << "\n";
			break;
		case LT:
			out << opString << " " << regStringSrc2 << ", "
			    << regStringSrc1 << "\n";
			break;
		case GT:
			out << opString << " " << regStringSrc2 << ", "
			    << regStringSrc1 << "\n";
			break;
		case OR:
			out << opString << " " << regStringSrc1 << ", "
			    << regStringSrc2 << "\n";
			break;
		case AND:
			out << opString << " " << regStringSrc1 << ", "
			    << regStringSrc2 << "\n";
			break;
		default:
			break;
	}
	/* clean up with setl if necessary */
	switch(op) {
		case ADD: break;
		case SUB: break;
		case DIV: break;
		case MULT: break;
		case EQ:
			codegen_indent(out);
			out << "sete %bl" << "\n";
			break;
		case NEQ:
			codegen_indent(out);
			out << "setne %bl" << "\n";
			break;
		case GTE:
			codegen_indent(out);
			out << "setge %bl" << "\n";
			break;
		case LTE:
			codegen_indent(out);
			out << "setle %bl" << "\n";
			break;
		case LT:
			codegen_indent(out);
			out << "setl %bl" << "\n";
			break;
		case GT:
			codegen_indent(out);
			out << "setg %bl" << "\n";
			break;
		case OR: break;
		case AND: break;
		default:
			break;
	}
	dst->genStore(out, regDstString);
}

void UnaryOpQuad::codegenX64(std::ostream& out){
	switch(op) {
		case NEG:
			src->genLoad(out, "%rbx");
			codegen_indent(out);
			out << "negq " << "%rbx\n";
			dst->genStore(out, "%rbx");
			break;
		case NOT:
			src->genLoad(out, "%rbx");
			codegen_indent(out);
			out << "notq " << "%rbx\n";
			dst->genStore(out, "%rbx");
			src->genLoad(out, "%rbx");
			codegen_indent(out);
			out << "andq " << dst->getMemoryLoc() << ", %rbx\n";
			dst->genStore(out, "%rbx");
			break;
	}
}

void AssignQuad::codegenX64(std::ostream& out){
	src->genLoad(out, "%rbx");
	dst->genStore(out, "%rbx");
}

void LocQuad::codegenX64(std::ostream& out){
	// (Optional)
}

void JmpQuad::codegenX64(std::ostream& out){
	codegen_indent(out);
	out << "jmp " << tgt->toString() << "\n";
}

void JmpIfQuad::codegenX64(std::ostream& out){
	cnd->genLoad(out, "%rbx");
	codegen_indent(out);
	out << "cmpq $0, %rbx\n";
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
	if(index >= 7) {
		opd->genLoad(out, "%rbx");
		codegen_indent(out);
		out << "pushq " << "%rbx" << "\n";
		return;
	}
	switch (index) {
		case 1:
			opd->genLoad(out, "%rdi");
			break;
		case 2:
			opd->genLoad(out, "%rsi");
			break;
		case 3:
			opd->genLoad(out, "%rdx");
			break;
		case 4:
			opd->genLoad(out, "%rcx");
			break;
		case 5:
			opd->genLoad(out, "%r8");
			break;
		case 6:
			opd->genLoad(out, "%r9");
			break;
	}
}

void GetArgQuad::codegenX64(std::ostream& out){
	//We don't actually need to do anything here
	/* OR: Move values from each register to the activation record */
	switch(index) {
		case 1:
			opd->genStore(out, "%rdi");
			break;
		case 2:
			opd->genStore(out, "%rsi");
			break;
		case 3:
			opd->genStore(out, "%rdx");
			break;
		case 4:
			opd->genStore(out, "%rcx");
			break;
		case 5:
			opd->genStore(out, "%r8");
			break;
		case 6:
			opd->genStore(out, "%r9");
			break;
                default: break;
	}
}

void SetRetQuad::codegenX64(std::ostream& out) {
	opd->genLoad(out, "%rax");
}

void GetRetQuad::codegenX64(std::ostream& out){
	opd->genStore(out, "%rax");
}

void SymOpd::genLoad(std::ostream & out, std::string regStr){
	codegen_indent(out);
	out << "movq " << myLoc << ", " << regStr << "\n";
}

void SymOpd::genStore(std::ostream& out, std::string regStr){
	codegen_indent(out);
	out << "movq " << regStr << ", " << myLoc << "\n";
}

void AuxOpd::genLoad(std::ostream & out, std::string regStr){
	codegen_indent(out);
	out << "movq " << myLoc << ", " << regStr << "\n";
	//out << "AuxOpd::genLoad\n";
}

void AuxOpd::genStore(std::ostream& out, std::string regStr){
	codegen_indent(out);
	out << "movq " << regStr << ", " << myLoc << "\n";
	//out << "AuxOpd::genStore\n";
}

void LitOpd::genLoad(std::ostream & out, std::string regStr){
	// e.g. movq eax, val
	codegen_indent(out);
	if(this->getType() == CHAR) {
		int output_val = static_cast<int>(val.at(0));
		out << "movq $" << output_val << ", " << regStr << "\n";
	} else if(this->getType() == BOOL) {
		int output_val = static_cast<int>(val.at(0)) - 48;
		out << "movq $" << output_val << ", " << regStr << "\n";
	} else {
		out << "movq $" << val << ", " << regStr << "\n";
	}
}

void LitOpd::genStore(std::ostream& out, std::string regStr){
	throw new InternalError("Cannot use literal as l-val");
}

}
