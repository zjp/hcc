#include "ast.hpp"
#include "symbol_table.hpp"
#include "errors.hpp"

namespace holeyc{

static void doIndent(std::ostream& out, int indent){
	for (int k = 0 ; k < indent; k++){ out << "\t"; }
}

void FnSymbol::unparse(std::ostream& out) {
	out << "(";
	for(auto type : formalTypes) {
	}
	out << "->";
	out << this->getType(); // return type
	out << ")";
}

void VarSymbol::unparse(std::ostream& out) {
	out << "(";
	out << this->getType();
	out << ")";
}

void ProgramNode::unparse(std::ostream& out, int indent, bool na) {
	for (DeclNode * decl : *myGlobals){
		decl->unparse(out, indent, na);
	}
}

void VarDeclNode::unparse(std::ostream& out, int indent, bool na) {
	doIndent(out, indent);
	myType->unparse(out, 0, na);
	out << " ";
	myID->unparse(out, 0, na);
	out << ";\n";
}

void FormalDeclNode::unparse(std::ostream& out, int indent, bool na) {
	doIndent(out, indent);
	getTypeNode()->unparse(out, 0, na);
	out << " ";
	ID()->unparse(out, 0, na);
}

void FnDeclNode::unparse(std::ostream& out, int indent, bool na) {
	doIndent(out, indent);
	myRetType->unparse(out, 0, na);
	out << " ";
	myID->unparse(out, 0, na);
	out << "(";
	bool firstFormal = true;
	for(auto formal : *myFormals){
		if (firstFormal) { firstFormal = false; }
		else { out << ","; }
		formal->unparse(out, 0, na);
	}
	out << "){\n";
	for(auto stmt : *myBody){
		stmt->unparse(out, indent+1, na);
	}
	doIndent(out, indent);
	out << "}\n";
}

void AssignStmtNode::unparse(std::ostream& out, int indent, bool na) {
	doIndent(out, indent);
	myExp->unparse(out,0,na);
	out << ";\n";
}

void FromConsoleStmtNode::unparse(std::ostream& out, int indent, bool na) {
	doIndent(out, indent);
	out << "FROMCONSOLE ";
	myDst->unparse(out,0,na);
	out << ";\n";
}

void ToConsoleStmtNode::unparse(std::ostream& out, int indent, bool na) {
	doIndent(out, indent);
	out << "TOCONSOLE ";
	mySrc->unparse(out,0,na);
	out << ";\n";
}

void PostIncStmtNode::unparse(std::ostream& out, int indent, bool na) {
	doIndent(out, indent);
	myLVal->unparse(out,0,na);
	out << "++;\n";
}

void PostDecStmtNode::unparse(std::ostream& out, int indent, bool na) {
	doIndent(out, indent);
	myLVal->unparse(out,0,na);
	out << "--;\n";
}

void IfStmtNode::unparse(std::ostream& out, int indent, bool na) {
	doIndent(out, indent);
	out << "if (";
	myCond->unparse(out, 0, na);
	out << "){\n";
	for (auto stmt : *myBody){
		stmt->unparse(out, indent + 1, na);
	}
	doIndent(out, indent);
	out << "}\n";
}

void IfElseStmtNode::unparse(std::ostream& out, int indent, bool na) {
	doIndent(out, indent);
	out << "if (";
	myCond->unparse(out, 0, na);
	out << "){\n";
	for (auto stmt : *myBodyTrue){
		stmt->unparse(out, indent + 1, na);
	}
	doIndent(out, indent);
	out << "} else {\n";
	for (auto stmt : *myBodyFalse){
		stmt->unparse(out, indent + 1, na);
	}
	doIndent(out, indent);
	out << "}\n";
}

void WhileStmtNode::unparse(std::ostream& out, int indent, bool na) {
	doIndent(out, indent);
	out << "while (";
	myCond->unparse(out, 0, na);
	out << "){\n";
	for (auto stmt : *myBody){
		stmt->unparse(out, indent + 1, na);
	}
	doIndent(out, indent);
	out << "}\n";
}

void ReturnStmtNode::unparse(std::ostream& out, int indent, bool na) {
	doIndent(out, indent);
	out << "return";
	if (myExp != nullptr){
		out << " ";
		myExp->unparse(out, 0, na);
	}
	out << ";\n";
}

void CallStmtNode::unparse(std::ostream& out, int indent, bool na) {
	doIndent(out, indent);
	myCallExp->unparse(out, 0, na);
	out << ";\n";
}

void ExpNode::unparseNested(std::ostream& out, bool na){
	out << "(";
	unparse(out, 0, na);
	out << ")";
}

void CallExpNode::unparse(std::ostream& out, int indent, bool na) {
	doIndent(out, indent);
	myID->unparse(out, 0, na);
	out << "(";

	bool firstArg = true;
	for(auto arg : *myArgs){
		if (firstArg) { firstArg = false; }
		else { out << ", "; }
		arg->unparse(out, 0, na);
	}
	out << ")";
}

void RefNode::unparse(std::ostream& out, int indent, bool na) {
	doIndent(out, indent);
	out << "^";
	myID->unparseNested(out, na);
}

void DerefNode::unparse(std::ostream& out, int indent, bool na) {
	doIndent(out, indent);
	out << "@";
	myID->unparseNested(out, na);
}

void IndexNode::unparse(std::ostream& out, int indent, bool na) {
	doIndent(out, indent);
	myBase->unparseNested(out, na);
	out << "[";
	myOffset->unparse(out, 0, na);
	out << "]";
}

void MinusNode::unparse(std::ostream& out, int indent, bool na) {
	doIndent(out, indent);
	myExp1->unparseNested(out, na);
	out << " - ";
	myExp2->unparseNested(out, na);
}

void PlusNode::unparse(std::ostream& out, int indent, bool na) {
	doIndent(out, indent);
	myExp1->unparseNested(out, na);
	out << " + ";
	myExp2->unparseNested(out, na);
}

void TimesNode::unparse(std::ostream& out, int indent, bool na) {
	doIndent(out, indent);
	myExp1->unparseNested(out, na);
	out << " * ";
	myExp2->unparseNested(out, na);
}

void DivideNode::unparse(std::ostream& out, int indent, bool na) {
	doIndent(out, indent);
	myExp1->unparseNested(out, na);
	out << " / ";
	myExp2->unparseNested(out, na);
}

void AndNode::unparse(std::ostream& out, int indent, bool na) {
	doIndent(out, indent);
	myExp1->unparseNested(out, na);
	out << " && ";
	myExp2->unparseNested(out, na);
}

void OrNode::unparse(std::ostream& out, int indent, bool na) {
	doIndent(out, indent);
	myExp1->unparseNested(out, na);
	out << " || ";
	myExp2->unparseNested(out, na);
}

void EqualsNode::unparse(std::ostream& out, int indent, bool na) {
	doIndent(out, indent);
	myExp1->unparseNested(out, na);
	out << " == ";
	myExp2->unparseNested(out, na);
}

void NotEqualsNode::unparse(std::ostream& out, int indent, bool na) {
	doIndent(out, indent);
	myExp1->unparseNested(out, na);
	out << " != ";
	myExp2->unparseNested(out, na);
}

void GreaterNode::unparse(std::ostream& out, int indent, bool na) {
	doIndent(out, indent);
	myExp1->unparseNested(out, na);
	out << " > ";
	myExp2->unparseNested(out, na);
}

void GreaterEqNode::unparse(std::ostream& out, int indent, bool na) {
	doIndent(out, indent);
	myExp1->unparseNested(out, na);
	out << " >= ";
	myExp2->unparseNested(out, na);
}

void LessNode::unparse(std::ostream& out, int indent, bool na) {
	doIndent(out, indent);
	myExp1->unparseNested(out, na);
	out << " < ";
	myExp2->unparseNested(out, na);
}

void LessEqNode::unparse(std::ostream& out, int indent, bool na) {
	doIndent(out, indent);
	myExp1->unparseNested(out, na);
	out << " <= ";
	myExp2->unparseNested(out, na);
}

void NotNode::unparse(std::ostream& out, int indent, bool na) {
	doIndent(out, indent);
	out << "!";
	myExp->unparseNested(out, na);
}

void NegNode::unparse(std::ostream& out, int indent, bool na) {
	doIndent(out, indent);
	out << "-";
	myExp->unparseNested(out, na);
}

void VoidTypeNode::unparse(std::ostream& out, int indent, bool na) {
	doIndent(out, indent);
	out << "void";
}

void IntTypeNode::unparse(std::ostream& out, int indent, bool na) {
	doIndent(out, indent);
	if (this->isPtr){
		out << "intptr";
	} else {
		out << "int";
	}
}

void BoolTypeNode::unparse(std::ostream& out, int indent, bool na) {
	doIndent(out, indent);
	if (this->isPtr){
		out << "boolptr";
	} else {
		out << "bool";
	}
}

void CharTypeNode::unparse(std::ostream& out, int indent, bool na) {
	doIndent(out, indent);
	if (this->isPtr){
		out << "charptr";
	} else {
		out << "char";
	}
}

void AssignExpNode::unparse(std::ostream& out, int indent, bool na) {
	doIndent(out, indent);
	myDst->unparseNested(out, na);
	out << " = ";
	mySrc->unparseNested(out, na);
}

void LValNode::unparseNested(std::ostream& out, bool na){
	unparse(out, 0, na);
}

void IDNode::unparse(std::ostream& out, int indent, bool na) {
	doIndent(out, indent);
	out << name;
	if (na && (mySymbol != nullptr)) {
		mySymbol->unparse(out);
	}
}

void IntLitNode::unparse(std::ostream& out, int indent, bool na) {
	doIndent(out, indent);
	out << myNum;
}

void CharLitNode::unparse(std::ostream& out, int indent, bool na) {
	doIndent(out, indent);
	if (myVal == '\n'){
		out << "'\\n";
	} else if (myVal == '\t'){
		out << "'\\t";
	} else {
		out << "'" << myVal;
	}
}

void StrLitNode::unparse(std::ostream& out, int indent, bool na) {
	doIndent(out, indent);
	out << myStr;
}

void NullPtrNode::unparse(std::ostream& out, int indent, bool na) {
	doIndent(out, indent);
	out << "NULLPTR";
}

void FalseNode::unparse(std::ostream& out, int indent, bool na) {
	doIndent(out, indent);
	out << "false";
}

void TrueNode::unparse(std::ostream& out, int indent, bool na) {
	doIndent(out, indent);
	out << "true";
}

} //End namespace holeyc
