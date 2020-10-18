#include "ast.hpp"
#include "symbol_table.hpp"
#include "errors.hpp"
#include "errType.hpp"
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
	ta->nodeType(this, BasicType::produce(VOID));
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
	TODO("Do FormalDeclNodes need to be checked against the function's formalDecls?");
}

void FnDeclNode::typeAnalysis(TypeAnalysis * ta) {

	//HINT: you might want to change the signature for
	// typeAnalysis on FnBodyNode to take a second
	// argument which is the type of the current 
	// function. This will help you to know at a 
	// return statement whether the return type matches
	// the current function

	//Note: this function may need extra code
	
	for (auto formal : *myFormals) {
		formal->typeAnalysis(ta);
	}

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
	myDst->typeAnalysis(ta);
	const DataType * myType = ta->nodeType(myDst);
	if(myType == BasicType::produce(VOID)) {
	}
}

void ToConsoleStmtNode::typeAnalysis(TypeAnalysis * ta) {
	mySrc->typeAnalysis(ta);
	const DataType * myType = ta->nodeType(mySrc);
	if(myType == BasicType::produce(VOID)) {
		ta->nodeType(this, ErrorType::produce());
		return;
	}
}

void PostDecStmtNode::typeAnalysis(TypeAnalysis * ta) {
	myLVal->typeAnalysis(ta);
	const DataType * myType = ta->nodeType(myLVal);
	if(myType != BasicType::produce(INT)) {
		ta->badMathOpr(this->line(), this->col());
		ta->nodeType(this, ErrorType::produce());
		return;
	}
}

void PostIncStmtNode::typeAnalysis(TypeAnalysis * ta) {
	myLVal->typeAnalysis(ta);
	const DataType * myType = ta->nodeType(myLVal);
	if(myType != BasicType::produce(INT)) {
		ta->badMathOpr(this->line(), this->col());
		ta->nodeType(this, ErrorType::produce());
		return;
	}
}

void IfStmtNode::typeAnalysis(TypeAnalysis * ta) {
	myCond->typeAnalysis(ta);
	const DataType * myCondType = ta->nodeType(myCond);
	if(myCondType != BasicType::produce(BOOL)) {
		ta->badIfCond(myCond->line(), myCond->col());
		ta->nodeType(this, ErrorType::produce());
		return;
	}
	for(auto stmt : *myBody) {
		stmt->typeAnalysis(ta);
	}
	// TODO: IfStmtNode: Does this node type need to be set to anything?
}

void IfElseStmtNode::typeAnalysis(TypeAnalysis * ta) {
	myCond->typeAnalysis(ta);
	const DataType * myCondType = ta->nodeType(myCond);
	if(myCondType != BasicType::produce(BOOL)) {
		ta->badIfCond(myCond->line(), myCond->col());
		ta->nodeType(this, ErrorType::produce());
		return;
	}
	for(auto stmt : *myBodyTrue) {
		stmt->typeAnalysis(ta);
	}
	for(auto stmt : *myBodyFalse) {
		stmt->typeAnalysis(ta);
	}
	// TODO: IfStmtNode: Does this node type need to be set to anything?
}

void WhileStmtNode::typeAnalysis(TypeAnalysis * ta) {
	myCond->typeAnalysis(ta);
	const DataType * myCondType = ta->nodeType(myCond);
	if(myCondType != BasicType::produce(BOOL)) {
		ta->badWhileCond(myCond->line(), myCond->col());
		ta->nodeType(this, ErrorType::produce());
		return;
	}
	for(auto stmt : *myBody) {
		stmt->typeAnalysis(ta);
	}
	// TODO: WhileStmtNode: Does this node type need to be set to anything?
}

// Currently causes a segfault
void ReturnStmtNode::typeAnalysis(TypeAnalysis * ta) {
	//const auto currFnType = ta->getCurrentFnType();
	//if(currFnType->asBasic() == BasicType::produce(VOID)) {
	//	ta->extraRetValue(myExp->line(), myExp->col());
	//}
	//myExp->typeAnalysis(ta);
	//const DataType * myType = ta->nodeType(myExp);
	//if(myType == currFnType) {
	//	return;
	//}
	//ta->badRetValue(myExp->line(), myExp->col());
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

void BinaryExpNode::typeAnalysis(TypeAnalysis * ta){
	myExp1->typeAnalysis(ta);
	myExp2->typeAnalysis(ta);
    const DataType * LHSType = ta->nodeType(myExp1);
	const DataType * RHSType = ta->nodeType(myExp2);

	//While incomplete, this gives you one case for 
	// assignment: if the types are exactly the same
	// it is usually ok to do the assignment. One
	// exception is that if both types are function
	// names, it should fail type analysis
	if (LHSType == RHSType){
		ta->nodeType(this, LHSType);
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

void PlusNode::typeAnalysis(TypeAnalysis * ta) {
    myExp1->typeAnalysis(ta);
	myExp2->typeAnalysis(ta);
	const DataType * exp1Type = ta->nodeType(myExp1);
	const DataType * exp2Type = ta->nodeType(myExp2);
    if(exp1Type->asError()||exp2Type->asError()){
        ta->nodeType(this, ErrorType::produce());
    }
    else if((exp1Type->isInt())&&(exp2Type->isInt()))
        ta->nodeType(this, BasicType::produce(INT));
    else{
        ta->badMathOpd(this->line(), this->col());
        ta->nodeType(this, ErrorType::produce());
    }
}

void MinusNode::typeAnalysis(TypeAnalysis * ta) {
    myExp1->typeAnalysis(ta);
	myExp2->typeAnalysis(ta);
	const DataType * exp1Type = ta->nodeType(myExp1);
	const DataType * exp2Type = ta->nodeType(myExp2);
    if(exp1Type->asError()||exp2Type->asError()){
        ta->nodeType(this, ErrorType::produce());
    }
    else if((exp1Type->isInt())&&(exp2Type->isInt()))
        ta->nodeType(this, BasicType::produce(INT));
    else{
        ta->badMathOpd(this->line(), this->col());
        ta->nodeType(this, ErrorType::produce());
    }
}

void TimesNode::typeAnalysis(TypeAnalysis * ta) {
    myExp1->typeAnalysis(ta);
	myExp2->typeAnalysis(ta);
	const DataType * exp1Type = ta->nodeType(myExp1);
	const DataType * exp2Type = ta->nodeType(myExp2);
    if(exp1Type->asError()||exp2Type->asError()){
        ta->nodeType(this, ErrorType::produce());
    }
    else if((exp1Type->isInt())&&(exp2Type->isInt()))
        ta->nodeType(this, BasicType::produce(INT));
    else{
        ta->badMathOpd(this->line(), this->col());
        ta->nodeType(this, ErrorType::produce());
    }
}
void DivideNode::typeAnalysis(TypeAnalysis * ta) {
    myExp1->typeAnalysis(ta);
	myExp2->typeAnalysis(ta);
    const DataType * exp1Type = ta->nodeType(myExp1);
    const DataType * exp2Type = ta->nodeType(myExp2);
    if(exp1Type->asError()||exp2Type->asError()){
        ta->nodeType(this, ErrorType::produce());
    }
    else if((exp1Type->isInt())&&(exp2Type->isInt()))
        ta->nodeType(this, BasicType::produce(INT));
    else{
        ta->badMathOpd(this->line(), this->col());
        ta->nodeType(this, ErrorType::produce());
    }
}

void AndNode::typeAnalysis(TypeAnalysis * ta) {
    myExp1->typeAnalysis(ta);
	myExp2->typeAnalysis(ta);
	const DataType * exp1Type = ta->nodeType(myExp1);
	const DataType * exp2Type = ta->nodeType(myExp2);
    if(exp1Type->asError()||exp2Type->asError()){
        ta->nodeType(this, ErrorType::produce());
    }
    else if((exp1Type->isBool())&&(exp2Type->isBool()))
        ta->nodeType(this, BasicType::produce(BOOL));
    else{
        ta->badLogicOpd(this->line(), this->col());
        ta->nodeType(this, ErrorType::produce());
    }
}

void OrNode::typeAnalysis(TypeAnalysis * ta) {
    myExp1->typeAnalysis(ta);
	myExp2->typeAnalysis(ta);
	const DataType * exp1Type = ta->nodeType(myExp1);
	const DataType * exp2Type = ta->nodeType(myExp2);
    if(exp1Type->asError()||exp2Type->asError()){
        ta->nodeType(this, ErrorType::produce());
    }
    else if((exp1Type->isBool())&&(exp2Type->isBool()))
        ta->nodeType(this, BasicType::produce(BOOL));
    else{
        ta->badLogicOpd(this->line(), this->col());
        ta->nodeType(this, ErrorType::produce());
    }
}

void EqualsNode::typeAnalysis(TypeAnalysis * ta) {
    myExp1->typeAnalysis(ta);
	myExp2->typeAnalysis(ta);
	const DataType * exp1Type = ta->nodeType(myExp1);
	const DataType * exp2Type = ta->nodeType(myExp2);
    if(exp1Type->asFn()||exp2Type->asFn()){
        ta->badRelOpd(this->line(), this->col());
        //ta->fnEq(this->line(), this->col());
        ta->nodeType(this, ErrorType::produce());
        }
    else if(exp1Type->asError()||exp2Type->asError()){
        ta->nodeType(this, ErrorType::produce());
    }
    else if(exp1Type->isVoid()||exp2Type->isVoid()){
        ta->badRelOpd(this->line(), this->col());
        //ta->voidEq(this->line(), this->col());
	}
    else if(exp1Type->validVarType()&&exp2Type->validVarType()){
        ta->nodeType(this, BasicType::produce(BOOL));
    }

}

void NotEqualsNode::typeAnalysis(TypeAnalysis * ta) {
    myExp1->typeAnalysis(ta);
	myExp2->typeAnalysis(ta);
	const DataType * exp1Type = ta->nodeType(myExp1);
	const DataType * exp2Type = ta->nodeType(myExp2);
    if(exp1Type->asFn()||exp2Type->asFn()){
		TypeErr::fnEq(this->line(), this->col());
        //ta->fnEq(this->line(), this->col());
        ta->nodeType(this, ErrorType::produce());
        }
    else if(exp1Type->asError()||exp2Type->asError()){
        ta->nodeType(this, ErrorType::produce());
    }
    else if(exp1Type->isVoid()||exp2Type->isVoid()){
		TypeErr::voidEq(this->line(), this->col());
        //ta->voidEq(this->line(), this->col());
	}
	else if (exp1Type->validVarType()&&exp2Type->validVarType()){
        ta->nodeType(this, BasicType::produce(BOOL));
    }

}

void LessNode::typeAnalysis(TypeAnalysis * ta) {
	myExp1->typeAnalysis(ta);
	myExp2->typeAnalysis(ta);
	const DataType * exp1Type = ta->nodeType(myExp1);
	const DataType * exp2Type = ta->nodeType(myExp2);
    if(exp1Type->asError()||exp2Type->asError()){
        ta->nodeType(this, ErrorType::produce());
    }
    else if((exp1Type->isInt())&&(exp2Type->isInt()))
        ta->nodeType(this, BasicType::produce(BOOL));
    else{
        ta->badRelOpd(this->line(), this->col());
        ta->nodeType(this, ErrorType::produce());
    }
}

void LessEqNode::typeAnalysis(TypeAnalysis * ta) {
	myExp1->typeAnalysis(ta);
	myExp2->typeAnalysis(ta);
	const DataType * exp1Type = ta->nodeType(myExp1);
	const DataType * exp2Type = ta->nodeType(myExp2);
    if(exp1Type->asError()||exp2Type->asError()){
        ta->nodeType(this, ErrorType::produce());
    }
    else if((exp1Type->isInt())&&(exp2Type->isInt()))
        ta->nodeType(this, BasicType::produce(BOOL));
    else{
        ta->badRelOpd(this->line(), this->col());
        ta->nodeType(this, ErrorType::produce());
    ta->nodeType(this, BasicType::produce(BOOL));
	}
}

void GreaterNode::typeAnalysis(TypeAnalysis * ta) {
	myExp1->typeAnalysis(ta);
	myExp2->typeAnalysis(ta);
	const DataType * exp1Type = ta->nodeType(myExp1);
	const DataType * exp2Type = ta->nodeType(myExp2);
    if(exp1Type->asError()||exp2Type->asError()){
        ta->nodeType(this, ErrorType::produce());
    }
    else if((exp1Type->isInt())&&(exp2Type->isInt()))
        ta->nodeType(this, BasicType::produce(BOOL));
    else{
        ta->badRelOpd(this->line(), this->col());
        ta->nodeType(this, ErrorType::produce());
    }
}

void GreaterEqNode::typeAnalysis(TypeAnalysis * ta) {
	myExp1->typeAnalysis(ta);
	myExp2->typeAnalysis(ta);
	const DataType * exp1Type = ta->nodeType(myExp1);
	const DataType * exp2Type = ta->nodeType(myExp2);
    if(exp1Type->asError()||exp2Type->asError()){
        ta->nodeType(this, ErrorType::produce());
    }
    else if((exp1Type->isInt())&&(exp2Type->isInt()))
        ta->nodeType(this, BasicType::produce(BOOL));
    else{
        ta->badRelOpd(this->line(), this->col());
        ta->nodeType(this, ErrorType::produce());
    }
}

void UnaryExpNode::typeAnalysis(TypeAnalysis * ta) {
}

void NegNode::typeAnalysis(TypeAnalysis * ta) {
    myExp->typeAnalysis(ta);
    const DataType * expType = ta->nodeType(myExp);
    if(expType->asError()){
        ta->nodeType(this, ErrorType::produce());
    }
    else if(expType->isInt()){ 
        ta->nodeType(this, BasicType::produce(INT));
    }
    else{
        ta->badMathOpd(myExp->line(), myExp->col());
        ta->nodeType(this, ErrorType::produce());
    }
}

void NotNode::typeAnalysis(TypeAnalysis * ta) {
    myExp->typeAnalysis(ta);
    const DataType * expType = ta->nodeType(myExp);
    if(expType->asError()){
        ta->nodeType(this, ErrorType::produce());
    }
    else if(expType->isBool()){ 
        ta->nodeType(this, BasicType::produce(BOOL));
    }
    else{
        ta->badLogicOpd(myExp->line(), myExp->col());
        ta->nodeType(this, ErrorType::produce());
    }
}

void VoidTypeNode::typeAnalysis(TypeAnalysis * ta) {
	ta->nodeType(this, BasicType::produce(VOID));
}

void IntTypeNode::typeAnalysis(TypeAnalysis * ta) {
	ta->nodeType(this, BasicType::produce(VOID));
}

void BoolTypeNode::typeAnalysis(TypeAnalysis * ta) {
	ta->nodeType(this, BasicType::produce(VOID));
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
    if(tgtType->asError()||srcType->asError()){
        ta->nodeType(this, ErrorType::produce());
    }
    else if(tgtType->asFn()||srcType->asFn()){

		// if one or both are functions
		// then ta->bad
		TypeErr::fnAssign(this->line(), this->col());
        ta->nodeType(this, ErrorType::produce());
    }
	else if (tgtType == srcType) {
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
