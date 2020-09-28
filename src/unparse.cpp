#include "ast.hpp"
#include "errors.hpp"

namespace holeyc{

static void doIndent(std::ostream& out, int indent){
	for (int k = 0 ; k < indent; k++){ out << "\t"; }
}

void ProgramNode::unparse(std::ostream& out, int indent){
	for (DeclNode * decl : *myGlobals){
		decl->unparse(out, indent);
	}
}

void VarDeclNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent); 
	myType->unparse(out, 0);
	out << " ";
	myID->unparse(out, 0);
	out << ";\n";
}

void FormalDeclNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent); 
	getTypeNode()->unparse(out, 0);
	out << " ";
	ID()->unparse(out, 0);
}

void FnDeclNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent); 
	myRetType->unparse(out, 0); 
	out << " ";
	myID->unparse(out, 0);
	out << "(";
	bool firstFormal = true;
	for(auto formal : *myFormals){
		if (firstFormal) { firstFormal = false; }
		else { out << ", "; }
		formal->unparse(out, 0);
	}
	out << "){\n";
	for(auto stmt : *myBody){
		stmt->unparse(out, indent+1);
	}
	doIndent(out, indent);
	out << "}\n";
}

void AssignStmtNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	myExp->unparse(out,0);
	out << ";\n";
}

void FromConsoleStmtNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	out << "FROMCONSOLE ";
	myDst->unparse(out,0);
	out << ";\n";
}

void ToConsoleStmtNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	out << "TOCONSOLE ";
	mySrc->unparse(out,0);
	out << ";\n";
}

void PostIncStmtNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	myLVal->unparse(out,0);
	out << "++;\n";
}

void PostDecStmtNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	myLVal->unparse(out,0);
	out << "--;\n";
}

void IfStmtNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	out << "if (";
	myCond->unparse(out, 0);
	out << "){\n";
	for (auto stmt : *myBody){
		stmt->unparse(out, indent + 1);
	}
	doIndent(out, indent);
	out << "}\n";
}

void IfElseStmtNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	out << "if (";
	myCond->unparse(out, 0);
	out << "){\n";
	for (auto stmt : *myBodyTrue){
		stmt->unparse(out, indent + 1);
	}
	doIndent(out, indent);
	out << "} else {\n";
	for (auto stmt : *myBodyFalse){
		stmt->unparse(out, indent + 1);
	}
	doIndent(out, indent);
	out << "}\n";
}

void WhileStmtNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	out << "while (";
	myCond->unparse(out, 0);
	out << "){\n";
	for (auto stmt : *myBody){
		stmt->unparse(out, indent + 1);
	}
	doIndent(out, indent);
	out << "}\n";
}

void ReturnStmtNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	out << "return";
	if (myExp != nullptr){
		out << " ";
		myExp->unparse(out, 0);
	}
	out << ";\n";
}

void CallStmtNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	myCallExp->unparse(out, 0);
	out << ";\n";
}

void ExpNode::unparseNested(std::ostream& out){
	out << "(";
	unparse(out, 0);
	out << ")";
}

void CallExpNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	myID->unparse(out, 0);
	out << "(";
	
	bool firstArg = true;
	for(auto arg : *myArgs){
		if (firstArg) { firstArg = false; }
		else { out << ", "; }
		arg->unparse(out, 0);
	}
	out << ")";
}

void RefNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	out << "^";
	myID->unparseNested(out);
}

void DerefNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	out << "@";
	myID->unparseNested(out);
}

void IndexNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	myBase->unparseNested(out);
	out << "[";
	myOffset->unparse(out, 0);
	out << "]";
}

void MinusNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	myExp1->unparseNested(out); 
	out << " - ";
	myExp2->unparseNested(out);
}

void PlusNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	myExp1->unparseNested(out); 
	out << " + ";
	myExp2->unparseNested(out);
}

void TimesNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	myExp1->unparseNested(out); 
	out << " * ";
	myExp2->unparseNested(out);
}

void DivideNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	myExp1->unparseNested(out); 
	out << " / ";
	myExp2->unparseNested(out);
}

void AndNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	myExp1->unparseNested(out); 
	out << " && ";
	myExp2->unparseNested(out);
}

void OrNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	myExp1->unparseNested(out); 
	out << " || ";
	myExp2->unparseNested(out);
}

void EqualsNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	myExp1->unparseNested(out); 
	out << " == ";
	myExp2->unparseNested(out);
}

void NotEqualsNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	myExp1->unparseNested(out); 
	out << " != ";
	myExp2->unparseNested(out);
}

void GreaterNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	myExp1->unparseNested(out); 
	out << " > ";
	myExp2->unparseNested(out);
}

void GreaterEqNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	myExp1->unparseNested(out); 
	out << " >= ";
	myExp2->unparseNested(out);
}

void LessNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	myExp1->unparseNested(out); 
	out << " < ";
	myExp2->unparseNested(out);
}

void LessEqNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	myExp1->unparseNested(out); 
	out << " <= ";
	myExp2->unparseNested(out);
}

void NotNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	out << "!";
	myExp->unparseNested(out); 
}

void NegNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	out << "-";
	myExp->unparseNested(out); 
}

void VoidTypeNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	out << "void";
}

void IntTypeNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	if (this->isPtr){
		out << "intptr";
	} else {
		out << "int";
	}
}

void BoolTypeNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	if (this->isPtr){
		out << "boolptr";
	} else {
		out << "bool";
	}
}

void CharTypeNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	if (this->isPtr){
		out << "charptr";
	} else {
		out << "char";
	}
}

void AssignExpNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	myDst->unparseNested(out);
	out << " = ";
	mySrc->unparseNested(out);
}

void LValNode::unparseNested(std::ostream& out){
	unparse(out, 0);
}

void IDNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	out << name;
}

void IntLitNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	out << myNum;
}

void CharLitNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	if (myVal == '\n'){
		out << "'\\n";
	} else if (myVal == '\t'){
		out << "'\\t";
	} else {
		out << "'" << myVal;
	}
}

void StrLitNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	out << myStr;
}

void NullPtrNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	out << "NULLPTR";
}

void FalseNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	out << "false";
}

void TrueNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	out << "true";
}

} //End namespace holeyc
