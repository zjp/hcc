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

/**
 * If a declaration is both bad (e.g. non function declared void) and is multiply declared,
 * report the bad declaration error first, then the multiply declared error
 */
bool VarDeclNode::nameAnalysis(SymbolTable* symTab) {
	// If this var is already in the symbol table, then:
	// if the type of this VarDeclNode is void {
	// symTab->errBadType(this->line(), this->col())
	// }
	bool nameAnalysisOk = true;
        if(this->myType->getType() == "void") {
		symTab->errBadTpe(myType->line(), myType->col());
		nameAnalysisOk = false;
	}
	if(symTab->lookup(myID->getName())) {
		// Issue a multiple definition error
		symTab->errMultDef(myID->line(), myID->col());
		nameAnalysisOk = false;
	} else {
          symTab->insert(new VarSymbol("int", myID->getName()));
        }
        return nameAnalysisOk;
}

bool FnDeclNode::nameAnalysis(SymbolTable* symTab) {
	if(symTab->lookup(myID->getName())) {
		std::cout << "Was true" << std::endl;
		symTab->errMultDef(myID->line(), myID->col());
	}
	//for (auto formal : myFormals ) {
	//	nameAnalysisOk = formal->nameAnalysis(symTab) && nameAnalysisOk;
	//}
	// Add scope for functio
	//symTab->add_scope();
	//for (auto stmt : myBody) {
	//	nameAnalysisOk = stmt->nameAnalysis(symTab) && nameAnalysisOk;
	//}
	return true;
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

}
