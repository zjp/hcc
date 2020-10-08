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
        if(myType->getType() == "void") {
		symTab->errBadTpe(myType->line(), myType->col());
		nameAnalysisOk = false;
	}
	if(symTab->lookup(myID->getName()) != nullptr) {
		symTab->errMultDef(myID->line(), myID->col());
		nameAnalysisOk = false;
	} else {
		symTab->insert(new VarSymbol(myType->getType(), myID->getName()));
        }
        return nameAnalysisOk;
}

/*
 * During name analysis, if a function name is doubly declared then we still need
 *
 */
bool FnDeclNode::nameAnalysis(SymbolTable* symTab) {
	bool nameAnalysisOk = true;
	// Check whether this function is already in the symbol table
	if(symTab->lookup(myID->getName()) != nullptr) {
		std::cout << "Was true" << std::endl;
		symTab->errMultDef(myID->line(), myID->col());
	}
	// Allocate a new scope table for the function
	symTab->add_scope();
	for (auto formal : *myFormals ) {
		nameAnalysisOk = formal->nameAnalysis(symTab) && nameAnalysisOk;
	}
	// Add scope for functio
	//symTab->add_scope();
	//for (auto stmt : myBody) {
	//	nameAnalysisOk = stmt->nameAnalysis(symTab) && nameAnalysisOk;
	//}
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
	std::cout << "I was called" << std::endl;
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

}
