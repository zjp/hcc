#include "ast.hpp"
#include "symbol_table.hpp"
#include "errors.hpp"

namespace holeyc {

//TODO: Add the rest of the nodes for nameAnalysis

bool ASTNode::nameAnalysis(SymbolTable* symTab){
	throw new ToDoError("This function should have"
		"been overriden in the subclass!");
}

bool ProgramNode::nameAnalysis(SymbolTable* symTab){
	bool res = true;
	symTab->add_scope();
	for (auto global : *myGlobals){
		res = global->nameAnalysis(symTab) && res;
	}
	return res;
}

bool VarDeclNode::nameAnalysis(SymbolTable* symTab) {
	bool nameAnalysisOk = true;
	bool need_to_add = true;
        if(myType->getType() == "void") {
		symTab->errBadTpe(myType->line(), myType->col());
		nameAnalysisOk = false;
		need_to_add = false;
	}
        if ((symTab->lookup(myID->getName()) == nullptr) && need_to_add) {
          symTab->insert(new VarSymbol(myType->getType(), myID->getName()));
        } else {
          symTab->errMultDef(myID->line(), myID->col());
          nameAnalysisOk = false;
        }
        return nameAnalysisOk;
}

/*
 * During name analysis, if a function name is doubly declared then we still need
 *
 */
bool FnDeclNode::nameAnalysis(SymbolTable* symTab) {
	bool nameAnalysisOk = true;
	// Don't add the function name to the symbol table if it's doubly declared
	bool need_to_add = true;
	// Check whether this function is already in the symbol table
	if(symTab->lookup(myID->getName()) != nullptr) {
		symTab->errMultDef(myID->line(), myID->col());
		need_to_add = false;
	}
	// Even if the function is doubly declared, we're still going to evaluate
	// the body
	// Allocate a new scope table for the function
	symTab->add_scope();
	for (auto formal : *myFormals ) {
		nameAnalysisOk = formal->nameAnalysis(symTab) && nameAnalysisOk;
	}
	for (auto stmt : *myBody) {
		nameAnalysisOk = stmt->nameAnalysis(symTab) && nameAnalysisOk;
	}
	symTab->drop_scope();
	return nameAnalysisOk;
}

bool WhileStmtNode::nameAnalysis(SymbolTable* symTab) {
	return true;
}

bool IfElseStmtNode::nameAnalysis(SymbolTable* symTab) {
	return true;
}

bool IfStmtNode::nameAnalysis(SymbolTable* symTab) {
	return true;
}

bool IDNode::nameAnalysis(SymbolTable* symTab) {
	bool nameAnalysisOk = true;
	SemSymbol* mySemanticSymbol = symTab->lookup(this->getName());
	if (mySemanticSymbol == nullptr) {
		symTab->errUndec(this->line(), this->col());
		nameAnalysisOk = false;
	} else {
		this->setSymbol(mySemanticSymbol);
	}
	return nameAnalysisOk;
}

bool BinaryExpNode::nameAnalysis(SymbolTable* symTab) {
	return myExp1->nameAnalysis(symTab) && myExp2->nameAnalysis(symTab);
}

bool UnaryExpNode::nameAnalysis(SymbolTable* symTab) {
	return myExp->nameAnalysis(symTab);
}

bool CallStmtNode::nameAnalysis(SymbolTable *symTab) {
	return myCallExp->nameAnalysis(symTab);
}

bool DerefNode::nameAnalysis(SymbolTable* symTab) {
	return myID->nameAnalysis(symTab);
}

bool AssignStmtNode::nameAnalysis(SymbolTable *symTab) {
	return myExp->nameAnalysis(symTab);
}

bool AssignExpNode::nameAnalysis(SymbolTable* symTab) {
	return myDst->nameAnalysis(symTab) && mySrc->nameAnalysis(symTab);
}

bool FromConsoleStmtNode::nameAnalysis(SymbolTable *symTab) {
	return myDst->nameAnalysis(symTab);
}

bool ToConsoleStmtNode::nameAnalysis(SymbolTable* symTab) {
	return mySrc->nameAnalysis(symTab);
}

bool PostDecStmtNode::nameAnalysis(SymbolTable *symTab) {
	return myLVal->nameAnalysis(symTab);
}

bool PostIncStmtNode::nameAnalysis(SymbolTable *symTab) {
	return myLVal->nameAnalysis(symTab);
}

bool IndexNode::nameAnalysis(SymbolTable* symTab) {
	return myBase->nameAnalysis(symTab) && myOffset->nameAnalysis(symTab);
}
bool RefNode::nameAnalysis(SymbolTable* symTab) {
	return myID->nameAnalysis(symTab);
}
bool CallExpNode::nameAnalysis(SymbolTable* symTab) {
	return true;
}
bool ReturnStmtNode::nameAnalysis(SymbolTable* symTab) {
	return myExp->nameAnalysis(symTab);
}

}
