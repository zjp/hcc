#include "ast.hpp"

namespace holeyc {

/*
 * doIndent is declared static, which means that it can
 * only be called in this file (its symbol is not exported).
 */
static void doIndent(std::ostream& out, int indent) {
	for (int k = 0 ; k < indent; k++) {
		out << "\t";
	}
}

/*
 * In this code, the intention is that functions are grouped
 * into files by purpose, rather than by class.
 * If you're used to having all of the functions of a class
 * defined in the same file, this style may be a bit disorienting,
 * though it is legal. Thus, we can have
 * ProgramNode::unparse, which is the unparse method of ProgramNodes
 * defined in the same file as DeclNode::unparse, the unparse method
 * of DeclNodes.
 */

void ProgramNode::unparse(std::ostream& out, int indent) {
	/* Oh, hey it's a for-each loop in C++!
	 * The loop iterates over each element in a collection
	 * without that gross i++ nonsense.
	 */
	for (auto global : *myGlobals){
		/* The auto keyword tells the compiler
		 * to (try to) figure out what the
		 * type of a variable should be from
		 * context. here, since we're iterating
		 * over a list of DeclNode *s, it's
		 * pretty clear that global is of
		 * type DeclNode *.
		 */
		global->unparse(out, indent);
	}
	out << "\n";
}

//void DeclNode::unparse(std::ostream& out, int indent) {
//	doIndent(out, indent);
//}

void VarDeclNode::unparse(std::ostream& out, int indent) {
	doIndent(out, indent);
	this->myType->unparse(out, 0);
	out << " ";
	this->myId->unparse(out, 0);
	out << ";\n";
}

void FnDeclNode::unparse(std::ostream& out, int indent) {
	doIndent(out, indent);
	this->myReturnType->unparse(out, 0);
	out << " ";
	this->myId->unparse(out, 0);
	out << "(";
	for (auto it = this->myFormals->begin(); it != this->myFormals->end(); ++it) {
		(*it)->unparse(out, 0);
		if(it != --this->myFormals->end()) {
			out << ", ";
		}
	}
	out << "){\n";
	for (auto it = this->myBody->begin(); it != this->myBody->end(); ++it) {
		(*it)->unparse(out, 1);
	}
	out << "}";
}

void StmtNode::unparse(std::ostream& out, int indent) {
	doIndent(out, indent);
}

void AssignStmtNode::unparse(std::ostream& out, int indent) {
	doIndent(out, indent);
	this->myExpr->unparse(out, indent);
	out << ";\n";
}

void IfStmtNode::unparse(std::ostream& out, int indent) {
	out << "if (";
	this->myExpr->unparse(out, indent);
	out << ") {";
	for (auto it = this->StmtList->begin(); it != this->StmtList->end(); ++it) {
		(*it)->unparse(out, 1);
	}
	out << "}";
}


void IfElseStmtNode::unparse(std::ostream& out, int indent) {
	out << "if ( ";
	this->myExpr->unparse(out, indent);
	out << " ) {";
	for (auto it = this->TrueStmtList->begin(); it != this->TrueStmtList->end(); ++it) {
		(*it)->unparse(out, 1);
	}
	out << " } ";
	out << "else { ";
	for (auto it = this->FalseStmtList->begin(); it != this->FalseStmtList->end(); ++it) {
		(*it)->unparse(out, 1);
	}
	out << " } ";

}

void WhileStmtNode::unparse(std::ostream& out, int indent) {
	out << "while (";
	this->myExpr->unparse(out, indent);
	out << ") {";
	for (auto it = this->StmtList->begin(); it != this->StmtList->end(); ++it) {
		(*it)->unparse(out, 1);
	}
	out << "}";
}

void FromConsoleStmtNode::unparse(std::ostream& out, int indent) {
	doIndent(out, indent);
	out << "FROMCONSOLE ";
	this->myExpr->unparse(out, indent);
	out << ";\n";
}

void ToConsoleStmtNode::unparse(std::ostream& out, int indent) {
	doIndent(out, indent);
	out << "TOCONSOLE ";
	this->myExpr->unparse(out, indent);
	out << ";\n";
}

void PostIncStmtNode::unparse(std::ostream& out, int indent) {
	doIndent(out, indent);
	this->myExpr->unparse(out, indent);
	out << "--;\n";
}
void PostDecStmtNode::unparse(std::ostream& out, int indent) {
	doIndent(out, indent);
	this->myExpr->unparse(out, indent);
	out << "++;\n";
}

void FormalDeclNode::unparse(std::ostream& out, int indent) {
	doIndent(out, indent);
	this->myType->unparse(out, 0);
	out<< " ";
	this->myId->unparse(out, 0);
}

void LValNode::unparse(std::ostream& out, int indent) {
	out << std::endl;
}

void AssignExpNode::unparse(std::ostream& out, int indent) {
	this->myLval->unparse(out, 0);
	out << " = ";
	this->mySrcExpr->unparse(out, 0);
}

void IDNode::unparse(std::ostream& out, int indent) {
	out << this->myStrVal;
}

void IntTypeNode::unparse(std::ostream& out, int indent) {
	out << "int";
	if(this->isRef) {
		out << "ptr";
	}
}

void CharTypeNode::unparse(std::ostream& out, int indent) {
	out << "char";
	if(this->isRef) {
		out << "ptr";
	}
}

void BoolTypeNode::unparse(std::ostream& out, int indent) {
	out << "bool";
	if(this->isRef) {
		out << "ptr";
	}
}

void ReturnStmtNode::unparse(std::ostream& out, int indent) {
	doIndent(out, indent);
	if(myExpr == nullptr) {
		out << "return;\n";
	} else {
		out << "return ";
		this->myExpr->unparse(out, indent);
		out << ";\n";
	}
}

void CallStmtNode::unparse(std::ostream& out, int indent) {
	doIndent(out, indent);
	this->myCallExpr->unparse(out, indent);
	out << ";\n";
}

void CallExpNode::unparse(std::ostream& out, int indent) {
	this->myId->unparse(out, indent);
	out << "(";
	for (auto it = this->myActuals->begin(); it != this->myActuals->end(); ++it) {
		(*it)->unparse(out, 1);
	}
	out << ")";
}

void DerefNode::unparse(std::ostream& out, int indent) {
	out << "@";
	this->myId->unparse(out, indent);
}

void IndexNode::unparse(std::ostream& out, int indent) {
	this->myId->unparse(out, indent);
	out << "[";
	this->myExp->unparse(out, indent);
	out << "]";
}

void RefNode::unparse(std::ostream& out, int indent) {
	out << "^";
	this->myId->unparse(out, indent);
}

void UnaryExpNode::unparse(std::ostream& out, int indent) {
	myExpr->unparse(out, indent);
}

void NegNode::unparse(std::ostream& out, int indent) {
	myExpr->unparse(out, indent);
}

void NotNode::unparse(std::ostream& out, int indent) {
	myExpr->unparse(out, indent);
}

void BinaryExpNode::unparse(std::ostream& out, int indent) {
	myLhs->unparse(out, indent);
	myRhs->unparse(out, indent);
}

void VoidTypeNode::unparse(std::ostream& out, int indent) {
	out << "void";
}

void AndNode::unparse(std::ostream& out, int indent) {
	myLhs->unparse(out, indent);
	out << " && ";
	myRhs->unparse(out, indent);
}

void DivideNode::unparse(std::ostream& out, int indent) {
	myLhs->unparse(out, indent);
	out << " / ";
	myRhs->unparse(out, indent);
}
void EqualsNode::unparse(std::ostream& out, int indent) {
	myLhs->unparse(out, indent);
	out << " == ";
	myRhs->unparse(out, indent);
}
void GreaterNode::unparse(std::ostream& out, int indent) {
	myLhs->unparse(out, indent);
	out << " > ";
	myRhs->unparse(out, indent);
}

void GreaterEqNode::unparse(std::ostream& out, int indent) {
	myLhs->unparse(out, indent);
	out << " >= ";
	myRhs->unparse(out, indent);
}

void LessEqNode::unparse(std::ostream& out, int indent) {
	myLhs->unparse(out, indent);
	out << " <= ";
	myRhs->unparse(out, indent);
}

void LessNode::unparse(std::ostream& out, int indent) {
	myLhs->unparse(out, indent);
	out << " < ";
	myRhs->unparse(out, indent);
}

void MinusNode::unparse(std::ostream& out, int indent) {
	myLhs->unparse(out, indent);
	out << " - ";
	myRhs->unparse(out, indent);
}

void NotEqualsNode::unparse(std::ostream& out, int indent) {
	myLhs->unparse(out, indent);
	out << " != ";
	myRhs->unparse(out, indent);
}

void OrNode::unparse(std::ostream& out, int indent) {
	myLhs->unparse(out, indent);
	out << " || ";
	myRhs->unparse(out, indent);
}

void TimesNode::unparse(std::ostream& out, int indent) {
	myLhs->unparse(out, indent);
	out << " * ";
	myRhs->unparse(out, indent);
}

void PlusNode::unparse(std::ostream& out, int indent) {
	myLhs->unparse(out, indent);
	out << " + ";
	myRhs->unparse(out, indent);
}

void CharLitNode::unparse(std::ostream& out, int indent) {
	out << this->myChar;
}

void FalseNode::unparse(std::ostream& out, int indent) {
	out << "false";
}

void IntLitNode::unparse(std::ostream& out, int indent) {
	out << this->myInt;
}

void NullPtrNode::unparse(std::ostream& out, int indent) {
	out << "NULL";
}

void StrLitNode::unparse(std::ostream& out, int indent) {
	out << this->myStr;
}

void TrueNode::unparse(std::ostream& out, int indent) {
	out << "true";
}

} // End namespace holeyc
