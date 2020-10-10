#include "ast.hpp"
#include "symbol_table.hpp"
#include "errName.hpp"
#include "types.hpp"

namespace holeyc{

bool ProgramNode::nameAnalysis(SymbolTable * symTab){
	//Enter the global scope
	symTab->enterScope();
	bool res = true;
	for (auto decl : *myGlobals){
		res = decl->nameAnalysis(symTab) && res;
	}
	//Leave the global scope
	symTab->leaveScope();
	return res;
}

bool AssignStmtNode::nameAnalysis(SymbolTable * symTab){
	return myExp->nameAnalysis(symTab);
}

bool PostIncStmtNode::nameAnalysis(SymbolTable * symTab){
	return myLVal->nameAnalysis(symTab);
}

bool PostDecStmtNode::nameAnalysis(SymbolTable * symTab){
	return myLVal->nameAnalysis(symTab);
}

bool FromConsoleStmtNode::nameAnalysis(SymbolTable * symTab){
	return myDst->nameAnalysis(symTab);
}

bool ToConsoleStmtNode::nameAnalysis(SymbolTable * symTab){
	return mySrc->nameAnalysis(symTab);
}

bool IfStmtNode::nameAnalysis(SymbolTable * symTab){
	bool result = true;
	result = myCond->nameAnalysis(symTab) && result;
	symTab->enterScope();
	for (auto stmt : *myBody){
		result = stmt->nameAnalysis(symTab) && result;
	}	
	symTab->leaveScope();
	return result;
}

bool IfElseStmtNode::nameAnalysis(SymbolTable * symTab){
	bool result = true;
	result = myCond->nameAnalysis(symTab) && result;
	symTab->enterScope();
	for (auto stmt : *myBodyTrue){
		result = stmt->nameAnalysis(symTab) && result;
	}	
	symTab->leaveScope();
	symTab->enterScope();
	for (auto stmt : *myBodyFalse){
		result = stmt->nameAnalysis(symTab) && result;
	}	
	symTab->leaveScope();
	return result;
}

bool WhileStmtNode::nameAnalysis(SymbolTable * symTab){
	bool result = true;
	result = myCond->nameAnalysis(symTab) && result;
	symTab->enterScope();
	for (auto stmt : *myBody){
		result = stmt->nameAnalysis(symTab) && result;
	}	
	symTab->leaveScope();
	return result;
}

bool VarDeclNode::nameAnalysis(SymbolTable * symTab){
	DataType * dataType = getTypeNode()->getType();
	std::string varName = ID()->getName();

	bool validType = dataType->validVarType();
	if (!validType){
		NameErr::badVarType(line(), col()); 
	}

	bool validName = !symTab->clash(varName);
	if (!validName){ 
		NameErr::multiDecl(ID()->line(), ID()->col()); 
	}

	if (!validType || !validName){ 
		return false; 
	} else {
		symTab->insert(new VarSymbol(varName, dataType));
		return true;
	}
}

bool FnDeclNode::nameAnalysis(SymbolTable * symTab){
	std::string fnName = this->ID()->getName();

	bool validRet = myRetType->nameAnalysis(symTab);

	// hold onto the scope of the function.
	ScopeTable * atFnScope = symTab->getCurrentScope();
	//Enter a new scope for "within" this function.
	ScopeTable * inFnScope = symTab->enterScope();

	/*Note that we check for a clash of the function 
	  name in it's declared scope (e.g. a global
	  scope for a global function)
	*/
	bool validName = true;
	if (atFnScope->clash(fnName)){
		NameErr::multiDecl(ID()->line(), ID()->col()); 
		validName = false;
	}

	bool validFormals = true;
	std::list<const DataType *> * formalTypes = 
		new std::list<const DataType *>();
	for (auto formal : *(this->myFormals)){
		validFormals = formal->nameAnalysis(symTab) && validFormals;
		TypeNode * typeNode = formal->getTypeNode();
		const DataType * formalType = typeNode->getType();
		formalTypes->push_back(formalType);
	}


	const DataType * retType = this->getRetTypeNode()->getType();
	FnType * dataType = new FnType(formalTypes, retType);
	//Make sure the fnSymbol is in the symbol table before 
	// analyzing the body, to allow for recursive calls
	if (validName){
		atFnScope->addFn(fnName, dataType);
	}

	bool validBody = true;
	for (auto stmt : *myBody){
		validBody = stmt->nameAnalysis(symTab) && validBody;
	}

	symTab->leaveScope();
	return (validRet && validFormals && validName && validBody);
}

bool RefNode::nameAnalysis(SymbolTable * symTab){
	return myID->nameAnalysis(symTab);
}

bool DerefNode::nameAnalysis(SymbolTable * symTab){
	return myID->nameAnalysis(symTab);
}

bool IndexNode::nameAnalysis(SymbolTable * symTab){
	bool res = true;
	res = myBase->nameAnalysis(symTab) && res;
	res = myOffset->nameAnalysis(symTab) && res;
	return res;
}

bool BinaryExpNode::nameAnalysis(SymbolTable * symTab){
	bool resultLHS = myExp1->nameAnalysis(symTab);
	bool resultRHS = myExp2->nameAnalysis(symTab);
	return resultLHS && resultRHS;
}

bool CallExpNode::nameAnalysis(SymbolTable* symTab){
	bool result = true;
	result = myID->nameAnalysis(symTab) && result;
	for (auto arg : *myArgs){
		result = arg->nameAnalysis(symTab) && result;
	}
	return result;
}

bool NegNode::nameAnalysis(SymbolTable* symTab){
	return myExp->nameAnalysis(symTab);
}

bool NotNode::nameAnalysis(SymbolTable* symTab){
	return myExp->nameAnalysis(symTab);
}

bool AssignExpNode::nameAnalysis(SymbolTable* symTab){
	bool result = true;
	result = myDst->nameAnalysis(symTab) && result;
	result = mySrc->nameAnalysis(symTab) && result;
	return result;
}

bool ReturnStmtNode::nameAnalysis(SymbolTable * symTab){
	if (myExp == nullptr){ // May happen in void functions
		return true;
	}
	return myExp->nameAnalysis(symTab);
}

bool CallStmtNode::nameAnalysis(SymbolTable* symTab){
	return myCallExp->nameAnalysis(symTab);
}

bool TypeNode::nameAnalysis(SymbolTable * symTab){
	return true;
}

bool IntLitNode::nameAnalysis(SymbolTable * symTab){
	return true;
}

bool CharLitNode::nameAnalysis(SymbolTable * symTab){
	return true;
}

bool StrLitNode::nameAnalysis(SymbolTable * symTab){
	return true;
}

bool NullPtrNode::nameAnalysis(SymbolTable * symTab){
	return true;
}

bool TrueNode::nameAnalysis(SymbolTable * symTab){
	return true;
}

bool FalseNode::nameAnalysis(SymbolTable * symTab){
	return true;
}

bool IDNode::nameAnalysis(SymbolTable* symTab){
	std::string myName = this->getName();
	SemSymbol * sym = symTab->find(myName);
	if (sym == nullptr){
		return NameErr::undeclID(line(), col());
	}
	this->attachSymbol(sym);
	return true;
}

void IDNode::attachSymbol(SemSymbol * symbolIn){
	this->mySymbol = symbolIn;
}

}
