#include <assert.h>

#include "name_analysis.hpp"
#include "type_analysis.hpp"

namespace holeyc {

TypeAnalysis * TypeAnalysis::build(NameAnalysis * nameAnalysis){
	TypeAnalysis * typeAnalysis = new TypeAnalysis();
	auto ast = nameAnalysis->ast;	
	typeAnalysis->ast = ast;

	ast->typeAnalysis(typeAnalysis);
	if (typeAnalysis->hasError){
		return nullptr;
	}

	return typeAnalysis;

}

void ProgramNode::typeAnalysis(TypeAnalysis * typing){
	for (auto decl : *myGlobals){
		decl->typeAnalysis(typing);
	}
	typing->nodeType(this, BasicType::VOID());
}

void IDNode::typeAnalysis(TypeAnalysis * typing){
	typing->nodeType(this, mySymbol->getDataType());
}

void VarDeclNode::typeAnalysis(TypeAnalysis * typing){
	myType->typeAnalysis(typing);
	const DataType * declaredType = typing->nodeType(myType);
	//We assume that the type that comes back is valid,
	// otherwise we wouldn't have passed nameAnalysis
	typing->nodeType(this, declaredType);
}

void FnDeclNode::typeAnalysis(TypeAnalysis * typing){
	myRetType->typeAnalysis(typing);
	const DataType * retDataType = typing->nodeType(myRetType);

	std::list<const DataType *> * formalTypes = 
		new std::list<const DataType *>();
	for (auto formal : *myFormals){
		formal->typeAnalysis(typing);
		formalTypes->push_back(typing->nodeType(formal));
	}	

	
	typing->nodeType(this, new FnType(formalTypes, retDataType));

	typing->setCurrentFnType(typing->nodeType(this)->asFn());
	for (auto stmt : *myBody){
		stmt->typeAnalysis(typing);
	}
	typing->setCurrentFnType(nullptr);
}

static const DataType * typeAssignOpd(TypeAnalysis * typing, ExpNode * opd){
	opd->typeAnalysis(typing);
	const DataType * type = typing->nodeType(opd);

	//Errors are invalid, but don't cause re-reports
	if (type->asError()){ return nullptr; }

	//Valid types are returned
	if (type->asBasic()){ 
		if (type->isVoid()){
			typing->badAssignOpd(opd->line(),opd->col());
			return nullptr;
		}
		return type; 
	}

	//Valid types are returned
	if (const PtrType * asPtr = type->asPtr()){ 
		return type; 
	}

	//Invalid types are reported and skip operator check
	typing->badAssignOpd(opd->line(),opd->col());
	return nullptr;
}

void AssignExpNode::typeAnalysis(TypeAnalysis * typing){
	const DataType * dstType = typeAssignOpd(typing, myDst);
	const DataType * srcType = typeAssignOpd(typing, mySrc);

	if (!dstType || !srcType){
		typing->nodeType(this, ErrorType::produce());
		return;
	}

	if (dstType == srcType){
		typing->nodeType(this, dstType);
		return;
	}

	//Allow any ptr type to recieve a nullptr
	if (dstType->asPtr()){
		if (const PtrType * srcPtr = srcType->asPtr()){
			if (srcPtr->decLevel() == BasicType::VOID()){
				typing->nodeType(this, dstType);
				return;
			}
		}
	}

	typing->badAssignOpr(line(), col());
	typing->nodeType(this, ErrorType::produce());
	return;
}

void CallExpNode::typeAnalysis(TypeAnalysis * typing){

	std::list<const DataType *> * aList = new std::list<const DataType *>();
	for (auto actual : *myArgs){
		actual->typeAnalysis(typing);
		aList->push_back(typing->nodeType(actual));
	}

	SemSymbol * calleeSym = myID->getSymbol();
	assert(calleeSym != nullptr);
	const DataType * calleeType = calleeSym->getDataType();
	const FnType * fnType = calleeType->asFn();
	if (fnType == nullptr){
		typing->badCallee(myID->line(), myID->col());
		typing->nodeType(this, ErrorType::produce());
		return;
	}

	const std::list<const DataType *>* fList = fnType->getFormalTypes();
	if (aList->size() != fList->size()){
		typing->badArgCount(line(), col());
		//Note: we still consider the call to return the 
		// return type
	} else {
		auto actualTypesItr = aList->begin();
		auto formalTypesItr = fList->begin();
		auto actualsItr = myArgs->begin();
		while(actualTypesItr != aList->end()){
			const DataType * actualType = *actualTypesItr;
			const DataType * formalType = *formalTypesItr;
			const ExpNode * actual = *actualsItr;
			actualTypesItr++;
			formalTypesItr++;
			actualsItr++;

			//Matching to error is ignored
			if (actualType->asError()){ continue; }
			if (formalType->asError()){ continue; }

			//Ok match
			if (formalType == actualType){ continue; }

			//Bad match
			typing->badArgMatch(actual->line(), actual->col());
			typing->nodeType(this, ErrorType::produce());
		}
	}

	typing->nodeType(this, fnType->getReturnType());
	return;
}

void NegNode::typeAnalysis(TypeAnalysis * typing){
	myExp->typeAnalysis(typing);
	const DataType * subType = typing->nodeType(myExp);

	//Propagate error, don't re-report
	if (subType->asError()){
		typing->nodeType(this, subType);
		return;
	}
	if (subType->isInt()){
		typing->nodeType(this, BasicType::INT());
	} else {
		typing->badMathOpd(myExp->line(), myExp->col());
		typing->nodeType(this, ErrorType::produce());
	}
}

void NotNode::typeAnalysis(TypeAnalysis * typing){
	myExp->typeAnalysis(typing);
	const DataType * childType = typing->nodeType(myExp);
	if (childType->isBool()){
		typing->nodeType(this, childType);
	} else if (childType->asError() != nullptr){
		typing->nodeType(this, ErrorType::produce());
		return;
	} else {
		typing->badLogicOpd(myExp->line(), myExp->col());
		typing->nodeType(this, ErrorType::produce());
	}
}

void VoidTypeNode::typeAnalysis(TypeAnalysis * typing){
	typing->nodeType(this, BasicType::VOID());
}

void IntTypeNode::typeAnalysis(TypeAnalysis * typing){
	BasicType * basic = BasicType::INT();
	if (isPtr){
		PtrType * ptr = PtrType::produce(basic, 1);
		typing->nodeType(this, ptr);
	} else {
		typing->nodeType(this, basic);
	}
}

void CharTypeNode::typeAnalysis(TypeAnalysis * typing){
	BasicType * basic = BasicType::produce(CHAR);
	if (isPtr){
		PtrType * ptr = PtrType::produce(basic, 1);
		typing->nodeType(this, ptr);
	} else {
		typing->nodeType(this, basic);
	}
}


void BoolTypeNode::typeAnalysis(TypeAnalysis * ta){
	BasicType * basic = BasicType::BOOL();
	if (isPtr){
		PtrType * ptr = PtrType::produce(basic, 1);
		ta->nodeType(this, ptr);
	} else {
		ta->nodeType(this, basic);
	}
}

static bool typeMathOpd(TypeAnalysis * typing, ExpNode * opd){
	opd->typeAnalysis(typing);
	const DataType * type = typing->nodeType(opd);
	if (type->isInt()){ return true; }
	if (type->asError()){
		//Don't re-report an error, but don't check for
		// incompatibility
		return false;
	}

	typing->badMathOpd(opd->line(), opd->col());
	return false;
}

void IndexNode::typeAnalysis(TypeAnalysis * typing){
	myBase->typeAnalysis(typing);
	myOffset->typeAnalysis(typing);

	const DataType * baseType = typing->nodeType(myBase);
	const DataType * offType = typing->nodeType(myOffset);

	if (offType->asError() || baseType->asError()){
		typing->nodeType(this, ErrorType::produce());
		return;
	}

	if (!offType->isInt()){
		typing->nodeType(this, ErrorType::produce());
		typing->badIndex(myOffset->line(), myOffset->col());
	}

	if (const PtrType * asPtr = baseType->asPtr()){
		typing->nodeType(this, asPtr->decLevel());
	} else {
		typing->badPtrBase(myBase->line(), myBase->col());
	}
}

void RefNode::typeAnalysis(TypeAnalysis * typeAnalysis){
	myID->typeAnalysis(typeAnalysis);
	const DataType * baseType = typeAnalysis->nodeType(myID);
	const DataType * errType = ErrorType::produce();

	if (baseType->asError()){
		typeAnalysis->nodeType(this, errType);
		return;
	}

	if (baseType->asPtr()){
		typeAnalysis->nodeType(this, errType);
		auto line = myID->line();
		auto col = myID->col();
		typeAnalysis->badRefOpd(line, col);
		return;
	}

	if (const DataType * asBasic = baseType->asBasic()){
		/* Syntactically disallowed to have ref return
		if (asBasic->isVoid()){
			auto line = myID->line();
			auto col = myID->col();
			typeAnalysis->badRefOpd(line, col);
		}
		*/
		const DataType * refType = PtrType::refType(baseType);
		typeAnalysis->nodeType(this, refType);
		return;
	}

	if (baseType->asFn()){
		typeAnalysis->nodeType(this, errType);
		auto line = myID->line();
		auto col = myID->col();
		typeAnalysis->badRefOpd(line, col);
	}
}

void DerefNode::typeAnalysis(TypeAnalysis * typeAnalysis){
	myID->typeAnalysis(typeAnalysis);
	const DataType * baseType = typeAnalysis->nodeType(myID);

	if (baseType->asError()){
		baseType = ErrorType::produce();
	}
	if (baseType->asFn()){
		typeAnalysis->fnDeref(this->line(), this->col());
		baseType = ErrorType::produce();
	}
	typeAnalysis->nodeType(this, PtrType::derefType(baseType));
}

void BinaryExpNode::binaryMathTyping(
	TypeAnalysis * typing
){
	bool lhsValid = typeMathOpd(typing, myExp1);
	bool rhsValid = typeMathOpd(typing, myExp2);
	if (!lhsValid || !rhsValid){
		typing->nodeType(this, ErrorType::produce());
		return;
	}

	//Get the valid operand types, check operator
	const DataType * lhsType = typing->nodeType(myExp1);
	const DataType * rhsType = typing->nodeType(myExp2);

	if (lhsType->isInt() && rhsType->isInt()){
		typing->nodeType(this, BasicType::INT());
		return;
	}
	typing->badMathOpr(this->line(), this->col());
	typing->nodeType(this, ErrorType::produce());
	return;
}

static const DataType * typeLogicOpd(
	TypeAnalysis * typing, ExpNode * opd
){
	opd->typeAnalysis(typing);
	const DataType * type = typing->nodeType(opd);

	//Return type if it's valid
	if (type->isBool()){ return type; }

	//Don't re-report an error, but return null to
	// indicate incompatibility
	if (type->asError()){ return nullptr; }

	//If type isn't an error, but is incompatible,
	// report and indicate incompatibility
	typing->badLogicOpd(opd->line(), opd->col());
	return NULL;
}

void BinaryExpNode::binaryLogicTyping(TypeAnalysis * typing){
	const DataType * lhsType = typeLogicOpd(typing, myExp1);
	const DataType * rhsType = typeLogicOpd(typing, myExp2);
	if (!lhsType || !rhsType){
		typing->nodeType(this, ErrorType::produce());
		return;
	}

	//Given valid operand types, check operator
	if (lhsType->isBool() && rhsType->isBool()){
		typing->nodeType(this, BasicType::BOOL());
		return;
	}

	//We never expect to get here, so we'll consider it
	// an error with the compiler itself
	throw new InternalError("Incomplete typing");
	typing->nodeType(this, ErrorType::produce());
	return;
}

void PlusNode::typeAnalysis(TypeAnalysis * typing){
	binaryMathTyping(typing);
}

void MinusNode::typeAnalysis(TypeAnalysis * typing){
	binaryMathTyping(typing);
}

void TimesNode::typeAnalysis(TypeAnalysis * typing){
	binaryMathTyping(typing);
}

void DivideNode::typeAnalysis(TypeAnalysis * typing){
	binaryMathTyping(typing);
}

void AndNode::typeAnalysis(TypeAnalysis * typing){
	binaryLogicTyping(typing);
}

void OrNode::typeAnalysis(TypeAnalysis * typing){
	binaryLogicTyping(typing);
}

static const DataType * typeEqOpd(
	TypeAnalysis * typing, ExpNode * opd
){
	opd->typeAnalysis(typing);
	const DataType * type = typing->nodeType(opd);

	if (type->isInt()){ return type; }
	if (type->isBool()){ return type; }
	if (type->isChar()){ return type; }

	if (type->isPtr()){
		return type;
	}

	//Errors are invalid, but don't cause re-reports
	if (type->asError()){ return nullptr; }

	typing->badEqOpd(opd->line(), opd->col());
	return nullptr;
}

void BinaryExpNode::binaryEqTyping(TypeAnalysis * typing){
	const DataType * lhsType = typeEqOpd(typing, myExp1);
	const DataType * rhsType = typeEqOpd(typing, myExp2);

	if (!lhsType || !rhsType){
		typing->nodeType(this, ErrorType::produce());
		return;
	}

	if (lhsType == rhsType){
		typing->nodeType(this, BasicType::BOOL());
		return;
	}

	const PtrType * lhsPtr = lhsType->asPtr();
	const PtrType * rhsPtr = rhsType->asPtr();
	if (lhsPtr && rhsPtr){
		typing->nodeType(this, BasicType::BOOL());
		return;
	}

	typing->badEqOpr(line(), col());
	typing->nodeType(this, ErrorType::produce());
	return;
}

void EqualsNode::typeAnalysis(TypeAnalysis * typing){
	binaryEqTyping(typing);
}

void NotEqualsNode::typeAnalysis(TypeAnalysis * typing){
	binaryEqTyping(typing);
}

static const DataType * typeRelOpd(
	TypeAnalysis * typing, ExpNode * opd
){
	opd->typeAnalysis(typing);
	const DataType * type = typing->nodeType(opd);

	if (type->isInt()){ return type; }

	//Errors are invalid, but don't cause re-reports
	if (type->asError()){ return nullptr; }

	typing->badRelOpd(opd->line(),opd->col());
	typing->nodeType(opd, ErrorType::produce());
	return nullptr;
}

void BinaryExpNode::binaryRelTyping(TypeAnalysis * typing){
	const DataType * lhsType = typeRelOpd(typing, myExp1);
	const DataType * rhsType = typeRelOpd(typing, myExp2);

	if (!lhsType || !rhsType){
		typing->nodeType(this, ErrorType::produce());
		return;
	}

	if (lhsType->isInt() && rhsType->isInt()){
		typing->nodeType(this, BasicType::BOOL());
		return;
	}
	//There is no bad relational operator, so we never 
	// expect to get here
	return;
}

void GreaterNode::typeAnalysis(TypeAnalysis * typing){
	binaryRelTyping(typing);
}

void GreaterEqNode::typeAnalysis(TypeAnalysis * typing){
	binaryRelTyping(typing);
}

void LessNode::typeAnalysis(TypeAnalysis * typing){
	binaryRelTyping(typing);
}

void LessEqNode::typeAnalysis(TypeAnalysis * typing){
	binaryRelTyping(typing);
}

void AssignStmtNode::typeAnalysis(TypeAnalysis * typing){
	myExp->typeAnalysis(typing);
	const DataType * childType = typing->nodeType(myExp);
	if (childType->asError()){
		typing->nodeType(this, ErrorType::produce());
	} else {
		typing->nodeType(this, BasicType::VOID());
	}
}

static const DataType * typeUnaryMath(
	size_t line, size_t col,
	TypeAnalysis * typing, ExpNode * exp
){
	exp->typeAnalysis(typing);
	const DataType * childType = typing->nodeType(exp);

	//Propagate error but don't re-report
	if (childType->asError()){
		return ErrorType::produce();
	}

	//It's fine to do unary math on an int
	if (childType->isInt()){
		return childType;
	}

	//Any other unary math is an error
	typing->badMathOpr(line, col);
	return ErrorType::produce();
}

void PostDecStmtNode::typeAnalysis(TypeAnalysis * typing){
	typing->nodeType(this, typeUnaryMath(this->line(), 
		this->col(), typing, myLVal));
}

void PostIncStmtNode::typeAnalysis(TypeAnalysis * typing){
	typing->nodeType(this, typeUnaryMath(this->line(), 
		this->col(), typing, myLVal));
}

void FromConsoleStmtNode::typeAnalysis(TypeAnalysis * typing){
	myDst->typeAnalysis(typing);
	const DataType * childType = typing->nodeType(myDst);
	const BasicType * childAsVar = childType->asBasic();

	if (childType->asBasic()){
		//Always ok
		return;
	} else if (childType->asPtr()){
		//Always bad
		typing->badWritePtr(myDst->line(), myDst->col());
		return;
	} else if (childType->asFn()){
		typing->badReadFn(myDst->line(),myDst->col());
		typing->nodeType(this, ErrorType::produce());
		return;
	} else {
		throw new InternalError("unaccounted-for type");
	}
	typing->nodeType(this, BasicType::VOID());
}

void ToConsoleStmtNode::typeAnalysis(TypeAnalysis * typing){
	mySrc->typeAnalysis(typing);
	const DataType * childType = typing->nodeType(mySrc);

	//Mark error, but don't re-report
	if (childType->asError()){
		typing->nodeType(this, ErrorType::produce());
		return;
	}

	//Check for invalid type
	if (childType->isVoid()){
		typing->badWriteVoid(mySrc->line(), mySrc->col());
		typing->nodeType(this, ErrorType::produce());
		return;
	} else if (childType->asFn()){
		typing->badWriteFn(mySrc->line(), mySrc->col());
		typing->nodeType(this, ErrorType::produce());
		return;
	} else if (childType->asBasic()){
		//Can write to a var of any other type
		return;
	}

	if (const PtrType * asPtr = childType->asPtr()){
		const DataType * deref = PtrType::derefType(asPtr);
		const BasicType * base = deref->asBasic();
		assert(base != nullptr);
			
		if (base->isChar()){
			typing->nodeType(this, BasicType::VOID());
		} else {
			size_t line = mySrc->line();
			size_t col = mySrc->line();
			typing->badWritePtr(line, col);
		}
		return;
	}

	typing->nodeType(this, BasicType::VOID());
}

void IfStmtNode::typeAnalysis(TypeAnalysis * typing){
	//Start off the typing as void, but may update to error
	typing->nodeType(this, BasicType::VOID());

	myCond->typeAnalysis(typing);
	const DataType * condType = typing->nodeType(myCond);
	bool goodCond = true;
	if (condType->asError()){
		typing->nodeType(this, ErrorType::produce());
		goodCond = false;
	} else if (!condType->isBool()){
		goodCond = false;
		typing->badIfCond(
			myCond->line(), 
			myCond->col());
		typing->nodeType(this, 
			ErrorType::produce());
	}

	for (auto stmt : *myBody){
		stmt->typeAnalysis(typing);
	}

	if (goodCond){
		typing->nodeType(this, BasicType::produce(VOID));
	} else {
		typing->nodeType(this, ErrorType::produce());
	}
}

void IfElseStmtNode::typeAnalysis(TypeAnalysis * typing){
	myCond->typeAnalysis(typing);
	const DataType * condType = typing->nodeType(myCond);

	bool goodCond = true;
	if (condType->asError()){
		goodCond = false;
		typing->nodeType(this, ErrorType::produce());
	} else if (!condType->isBool()){
		typing->badIfCond(myCond->line(), myCond->col());
		goodCond = false;
	}
	for (auto stmt : *myBodyTrue){
		stmt->typeAnalysis(typing);
	}
	for (auto stmt : *myBodyFalse){
		stmt->typeAnalysis(typing);
	}
	
	if (goodCond){
		typing->nodeType(this, BasicType::produce(VOID));
	} else {
		typing->nodeType(this, ErrorType::produce());
	}
}

void WhileStmtNode::typeAnalysis(TypeAnalysis * typing){
	myCond->typeAnalysis(typing);
	const DataType * condType = typing->nodeType(myCond);

	if (condType->asError()){
		typing->nodeType(this, ErrorType::produce());
	} else if (!condType->isBool()){
		typing->badWhileCond(myCond->line(), myCond->col());
	}

	for (auto stmt : *myBody){
		stmt->typeAnalysis(typing);
	}

	typing->nodeType(this, BasicType::VOID());
}

void CallStmtNode::typeAnalysis(TypeAnalysis * typing){
	myCallExp->typeAnalysis(typing);
	typing->nodeType(this, BasicType::VOID());
}

void ReturnStmtNode::typeAnalysis(TypeAnalysis * typing){
	const FnType * fnType = typing->getCurrentFnType();
	const DataType * fnRet = fnType->getReturnType();

	//Check: shouldn't return anything
	if (fnRet == BasicType::VOID()){
		if (myExp != nullptr) {
			myExp->typeAnalysis(typing);
			typing->extraRetValue(
				myExp->line(), 
				myExp->col()); 
			typing->nodeType(this, ErrorType::produce());
		} else {
			typing->nodeType(this, BasicType::VOID());
		}
		return;
	}

	//Check: returns nothing, but should
	if (myExp == nullptr){
			typing->badNoRet(line(), col());
			typing->nodeType(this, ErrorType::produce());
			return;
	}

	myExp->typeAnalysis(typing);
	const DataType * childType = typing->nodeType(myExp);

	if (childType->asError()){
		typing->nodeType(this, ErrorType::produce());
		return;
	}

	if (childType != fnRet){
		typing->badRetValue(myExp->line(), myExp->col());
		typing->nodeType(this, ErrorType::produce());
		return;
	}
	typing->nodeType(this, ErrorType::produce());
	return;
}

void CharLitNode::typeAnalysis(TypeAnalysis * typing){
	BasicType * basic = BasicType::CHAR();
	typing->nodeType(this, BasicType::CHAR());
}

void StrLitNode::typeAnalysis(TypeAnalysis * typing){
	BasicType * basic = BasicType::CHAR();
	PtrType * ptr = PtrType::produce(basic, 1);
	typing->nodeType(this, ptr);
}

void NullPtrNode::typeAnalysis(TypeAnalysis * typing){
	BasicType * basic = BasicType::VOID();
	PtrType * ptr = PtrType::produce(basic, 1);
	typing->nodeType(this, ptr);
}

void FalseNode::typeAnalysis(TypeAnalysis * typing){
	typing->nodeType(this, BasicType::BOOL());
}

void TrueNode::typeAnalysis(TypeAnalysis * typing){
	typing->nodeType(this, BasicType::BOOL());
}

void IntLitNode::typeAnalysis(TypeAnalysis * typing){
	typing->nodeType(this, BasicType::INT());
}

}
