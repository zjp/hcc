#include "ast.hpp"
#include "symbol_table.hpp"
#include "errors.hpp"
#include "types.hpp"
#include "name_analysis.hpp"
#include "type_analysis.hpp"

namespace holeyc{

TypeAnalysis * TypeAnalysis::build(NameAnalysis * nameAnalysis){
	//To emphasize that type analysis depends on name analysis
	// being complete, a name analysis must be supplied for 
	// type analysis to be performed.
	TypeAnalysis * typeAnalysis = new TypeAnalysis();
	auto ast = nameAnalysis->ast;	
	typeAnalysis->ast = ast;

	ast->typeAnalysis(typeAnalysis);
	if (typeAnalysis->hasError){
		return nullptr;
	}

	return typeAnalysis;

}

void ProgramNode::typeAnalysis(TypeAnalysis * ta){

	//pass the TypeAnalysis down throughout
	// the entire tree, getting the types for
	// each element in turn and adding them
	// to the ta object's hashMap
	for (auto global : *myGlobals){
		global->typeAnalysis(ta);
	}

	//The type of the program node will never
	// be needed. We can just set it to VOID
	//(Alternatively, we could make our type 
	// be error if the DeclListNode is an error)
	ta->nodeType(this, BasicType::produce(VOID));
}

void ExpNode::typeAnalysis(TypeAnalysis * ta){
	TODO("Override me in the subclass");
}

void LValNode::typeAnalysis(TypeAnalysis * ta) {
	TODO("Override me in the subclass");
}

void IDNode::typeAnalysis(TypeAnalysis * ta){
	// IDs never fail type analysis and always
	// yield the type of their symbol (which
	// depends on their definition)
	ta->nodeType(this, this->getSymbol()->getDataType());

}

void RefNode::typeAnalysis(TypeAnalysis * ta) {
}
void DerefNode::typeAnalysis(TypeAnalysis * ta) {
}
void IndexNode::typeAnalysis(TypeAnalysis * ta) {
}
void TypeNode::typeAnalysis(TypeAnalysis * ta) {
}

void CharTypeNode::typeAnalysis(TypeAnalysis * ta) {
}

void StmtNode::typeAnalysis(TypeAnalysis * ta) {
	TODO("Implement me in the subclass");
}

void DeclNode::typeAnalysis(TypeAnalysis * ta) {
	TODO("Override me in the subclass");
}

void VarDeclNode::typeAnalysis(TypeAnalysis * ta) {
	// VarDecls always pass type analysis, since they 
	// are never used in an expression. You may choose
	// to type them void (like this), as discussed in class
	ta->nodeType(this, BasicType::produce(VOID));
}

void FormalDeclNode::typeAnalysis(TypeAnalysis * ta) {
}

void FnDeclNode::typeAnalysis(TypeAnalysis * ta) {

	//HINT: you might want to change the signature for
	// typeAnalysis on FnBodyNode to take a second
	// argument which is the type of the current 
	// function. This will help you to know at a 
	// return statement whether the return type matches
	// the current function

	//Note: this function may need extra code

	for (auto stmt : *myBody){
		stmt->typeAnalysis(ta);
	}

}

void AssignStmtNode::typeAnalysis(TypeAnalysis * ta) {
	myExp->typeAnalysis(ta);

	//It can be a bit of a pain to write 
	// "const DataType *" everywhere, so here
	// the use of auto is used instead to tell the
	// compiler to figure out what the subType variable
	// should be
	auto subType = ta->nodeType(myExp);

	// As error returns null if subType is NOT an error type
	// otherwise, it returns the subType itself
	if (subType->asError()){
		ta->nodeType(this, subType);
	} else {
		ta->nodeType(this, BasicType::produce(VOID));
	}
}

void FromConsoleStmtNode::typeAnalysis(TypeAnalysis * ta) {
	TODO("FromConsoleStmtNode: Check type of myDst");
	TODO("FromConsoleStmtNode: Check type of incoming data");
}

void ToConsoleStmtNode::typeAnalysis(TypeAnalysis * ta) {
	TODO("ToConsoleStmtNode: Ensure type of mySrc is int, bool, or char");
}

void PostDecStmtNode::typeAnalysis(TypeAnalysis * ta) {
	TODO("PostDecStmtNode: Ensure myLVal is int");
}

void PostIncStmtNode::typeAnalysis(TypeAnalysis * ta) {
	TODO("PostIncStmtNode: Ensure myLVal is int");
}

void IfStmtNode::typeAnalysis(TypeAnalysis * ta) {
	myCond->typeAnalysis(ta);
	TODO("Ensure the condition is bool in IfStmtNode");
	for(auto stmt : *myBody) {
		stmt->typeAnalysis(ta);
	}
}

void IfElseStmtNode::typeAnalysis(TypeAnalysis * ta) {
	myCond->typeAnalysis(ta);
	TODO("Ensure the condition is bool in IfElseStmtNode");
	for(auto stmt : *myBodyTrue) {
		stmt->typeAnalysis(ta);
	}
	for(auto stmt : *myBodyFalse) {
		stmt->typeAnalysis(ta);
	}
}

void WhileStmtNode::typeAnalysis(TypeAnalysis * ta) {
	myCond->typeAnalysis(ta);
	TODO("Ensure the condition is bool in WhileStmtNode");
	for(auto stmt : *myBody) {
		stmt->typeAnalysis(ta);
	}
}

void ReturnStmtNode::typeAnalysis(TypeAnalysis * ta) {
	myExp->typeAnalysis(ta);
}

void CallExpNode::typeAnalysis(TypeAnalysis * ta) {
	myID->typeAnalysis(ta);
	TODO("Check whether myID is a function");
	// if not myID is a function:
	// else
	int i = 0;
	for(auto argt : *myArgs) {
		argt->typeAnalysis(ta);
		++i;
	}
	TODO("Check whether there are as many args as function requires");
}

/**
 * Binary Expression Block
 *
 * Arithmetic Expressions must have int on both sides 
 * Logical expressions must have bool on both sides ! && ||
 * Equality != ==
 * Relational >= <= > < 
 * Assignment = 
 *
 */

void BinaryExpNode::typeAnalysis(TypeAnalysis * ta) {
}

void PlusNode::typeAnalysis(TypeAnalysis * ta) {
	myExp1->typeAnalysis(ta);
	myExp2->typeAnalysis(ta);
	TODO("Check that both types are integer in PlusNode");
	ta->nodeType(this, BasicType::produce(INT));
}

void MinusNode::typeAnalysis(TypeAnalysis * ta) {
	myExp1->typeAnalysis(ta);
	myExp2->typeAnalysis(ta);
	TODO("Check that both types are integer in MinusNode");
	ta->nodeType(this, BasicType::produce(INT));
}

void TimesNode::typeAnalysis(TypeAnalysis * ta) {
	myExp1->typeAnalysis(ta);
	myExp2->typeAnalysis(ta);
	TODO("Check that both types are integer in TimesNode");
	ta->nodeType(this, BasicType::produce(INT));
}

void DivideNode::typeAnalysis(TypeAnalysis * ta) {
	myExp1->typeAnalysis(ta);
	myExp2->typeAnalysis(ta);
	TODO("Check that both types are integer in DivideNode");
	ta->nodeType(this, BasicType::produce(INT));
}

void AndNode::typeAnalysis(TypeAnalysis * ta) {
	myExp1->typeAnalysis(ta);
	myExp2->typeAnalysis(ta);
	TODO("Check that both types are boolean in AndNode");
	ta->nodeType(this, BasicType::produce(BOOL));
}

void OrNode::typeAnalysis(TypeAnalysis * ta) {
	myExp1->typeAnalysis(ta);
	myExp2->typeAnalysis(ta);
	TODO("Check that both types are boolean in OrNode");
	ta->nodeType(this, BasicType::produce(BOOL));
}

void EqualsNode::typeAnalysis(TypeAnalysis * ta) {
	myExp1->typeAnalysis(ta);
	myExp2->typeAnalysis(ta);
	TODO("Check that both types are the same type in EqualsNode");
	ta->nodeType(this, BasicType::produce(BOOL));
}

void NotEqualsNode::typeAnalysis(TypeAnalysis * ta) {
	myExp1->typeAnalysis(ta);
	myExp2->typeAnalysis(ta);
	TODO("Check that both types are the same type in NotEqualsNode");
	ta->nodeType(this, BasicType::produce(BOOL));
}

void LessNode::typeAnalysis(TypeAnalysis * ta) {
	myExp1->typeAnalysis(ta);
	myExp2->typeAnalysis(ta);
	TODO("Check that both types are the same type in LessNode");
	ta->nodeType(this, BasicType::produce(BOOL));
}

void LessEqNode::typeAnalysis(TypeAnalysis * ta) {
	myExp1->typeAnalysis(ta);
	myExp2->typeAnalysis(ta);
	TODO("Check that both types are the same type in LessEqNode");
	ta->nodeType(this, BasicType::produce(BOOL));
}

void GreaterNode::typeAnalysis(TypeAnalysis * ta) {
	myExp1->typeAnalysis(ta);
	myExp2->typeAnalysis(ta);
	TODO("Check that both types are the same type in GreaterNode");
	ta->nodeType(this, BasicType::produce(BOOL));
}

void GreaterEqNode::typeAnalysis(TypeAnalysis * ta) {
	myExp1->typeAnalysis(ta);
	myExp2->typeAnalysis(ta);
	TODO("Check that both types are the same type in GreaterEqNode");
	ta->nodeType(this, BasicType::produce(BOOL));
}

void UnaryExpNode::typeAnalysis(TypeAnalysis * ta) {
}

void NegNode::typeAnalysis(TypeAnalysis * ta) {
	myExp->typeAnalysis(ta);
	TODO("Check that the type of NegNode's exp is integer");
	ta->nodeType(this, BasicType::produce(INT));
}

void NotNode::typeAnalysis(TypeAnalysis * ta) {
	myExp->typeAnalysis(ta);
	TODO("Check that the type of NotNode's exp is boolean");
	ta->nodeType(this, BasicType::produce(BOOL));
}

void VoidTypeNode::typeAnalysis(TypeAnalysis * ta) {
}

void IntTypeNode::typeAnalysis(TypeAnalysis * ta) {
}

void BoolTypeNode::typeAnalysis(TypeAnalysis * ta) {

}

void AssignExpNode::typeAnalysis(TypeAnalysis * ta){
	//Do typeAnalysis on the subexpressions
	myDst->typeAnalysis(ta);
	mySrc->typeAnalysis(ta);

	const DataType * tgtType = ta->nodeType(myDst);
	const DataType * srcType = ta->nodeType(mySrc);

	//While incomplete, this gives you one case for 
	// assignment: if the types are exactly the same
	// it is usually ok to do the assignment. One
	// exception is that if both types are function
	// names, it should fail type analysis
	if (tgtType == srcType) {
		TODO("AssignExpNode: Check whether tgtType and srcType are functions");
		// if one or both are functions
		// then ta->bad
		ta->nodeType(this, tgtType);
		return;
	}
	
	//Some functions are already defined for you to
	// report type errors. Note that these functions
	// also tell the typeAnalysis object that the
	// analysis has failed, meaning that main.cpp
	// will print "Type check failed" at the end
	ta->badAssignOpr(this->line(), this->col());


	//Note that reporting an error does not set the
	// type of the current node, so setting the node
	// type must be done
	ta->nodeType(this, ErrorType::produce());
}

void IntLitNode::typeAnalysis(TypeAnalysis * ta){
	// IntLits never fail their type analysis and always
	// yield the type INT
	ta->nodeType(this, BasicType::produce(INT));
}

// Level?
void StrLitNode::typeAnalysis(TypeAnalysis * ta) {
	ta->nodeType(this, PtrType::produce(BasicType::produce(CHAR), 1));
}

void CharLitNode::typeAnalysis(TypeAnalysis * ta) {
	// StrLits should never fail type analysis either
	ta->nodeType(this, BasicType::produce(CHAR));
}

void NullPtrNode::typeAnalysis(TypeAnalysis * ta) {
	ta->nodeType(this, PtrType::produce(BasicType::produce(VOID), 1));
}

void TrueNode::typeAnalysis(TypeAnalysis * ta) {
	ta->nodeType(this, BasicType::produce(BOOL));
}

void FalseNode::typeAnalysis(TypeAnalysis * ta) {
	ta->nodeType(this, BasicType::produce(BOOL));
}

void CallStmtNode::typeAnalysis(TypeAnalysis * ta) {
}

}
