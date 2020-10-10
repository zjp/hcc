#ifndef XXLANG_DATA_TYPES
#define XXLANG_DATA_TYPES

#include <list>
#include <sstream>
#include "errors.hpp"

#include <unordered_map>

#ifndef HOLEYC_HASH_MAP_ALIAS
// Use an alias template so that we can use
// "HashMap" and it means "std::unordered_map"
template <typename K, typename V>
using HashMap = std::unordered_map<K, V>;
#endif


namespace holeyc{

class ASTNode;

class BasicType;
class FnType;
class PtrType;
class ErrorType;

enum BaseType{
	INT, VOID, BOOL, CHAR
};

//This class is the superclass for all holeyc types. You
// can get information about which type is implemented
// concretely using the as<X> functions, or query information
// using the is<X> functions.
class DataType{
public:
	virtual std::string getString() const = 0;
	virtual const BasicType * asBasic() const { return nullptr; }
	virtual const PtrType * asPtr() const { return nullptr; }
	virtual const FnType * asFn() const { return nullptr; }
	virtual const ErrorType * asError() const { return nullptr; }
	virtual bool isVoid() const { return false; }
	virtual bool isInt() const { return false; }
	virtual bool isBool() const { return false; }
	virtual bool isPtr() const { return false; }
	virtual bool validVarType() const = 0 ;
protected:
};

//This DataType subclass is the superclass for all holeyc types. 
// Note that there is exactly one instance of this 
class ErrorType : public DataType{
public:
	static ErrorType * produce(){
		//Note: this static member will only ever be initialized 
		// ONCE, no matter how many times the function is called.
		// That means there will only ever be 1 instance of errorType
		// in the entire codebase.
		static ErrorType * error = new ErrorType();
		
		return error;
	}
	virtual const ErrorType * asError() const override { return this; }
	virtual std::string getString() const override { 
		return "ERROR";
	}
	virtual bool validVarType() const override { return false; }
private:
	ErrorType(){ 
		/* private constructor, can only 
		be called from produce */
	}
	size_t line;
	size_t col;
};

//DataType subclass for all scalar types 
class BasicType : public DataType{
public:
	static BasicType * VOID(){
		return produce(BaseType::VOID);
	}
	static BasicType * BOOL(){
		return produce(BaseType::BOOL);
	}
	static BasicType * CHAR(){
		return produce(BaseType::CHAR);
	}
	static BasicType * INT(){
		return produce(BaseType::INT);
	}

	//Create a scalar type. If that type already exists,
	// return the known instance of that type. Making sure
	// there is only 1 instance of a class for a given set
	// of fields is known as the "flyweight" design pattern
	// and ensures that the memory needs of a program are kept
	// down: rather than having a distinct type for every base
	// INT (for example), only one is constructed and kept in
	// the flyweights list. That type is then re-used anywhere
	// it's needed. 

	//Note the use of the static function declaration, which 
	// means that no instance of BasicType is needed to call
	// the function.
	static BasicType * produce(BaseType base){
		//Note the use of the static local variable, which
		//means that the flyweights variable persists between
		// multiple calls to this function (it is essentially
		// a global variable that can only be accessed
		// in this function).
		static std::list<BasicType *> flyweights;
		for(BasicType * fly : flyweights){
			if (fly->getBaseType() == base){
				return fly;
			}
		}
		BasicType * newType = new BasicType(base);
		flyweights.push_back(newType);
		return newType;
	}
	const BasicType * asVar() const {
		return this;
	}
	BasicType * asVar(){
		return this;
	}
	bool isInt() const override {
		return myBaseType == BaseType::INT;
	}
	bool isBool() const override {
		return myBaseType == BaseType::BOOL;
	}
	virtual bool isVoid() const override { 
		return myBaseType == BaseType::VOID; 
	}
	virtual bool validVarType() const override {
		return !isVoid();
	}
	virtual BaseType getBaseType() const { return myBaseType; }
	virtual std::string getString() const override;
private:
	BasicType(BaseType base) 
	: myBaseType(base){ }
	BaseType myBaseType;
};

class PtrType : public DataType{
public:
	static PtrType * produce(const BasicType * basicType, int level){
		if (level <= 0){
			throw new InternalError("bad pointer level");
		}

		//Note the use of the static local variable, which
		//means that the flyweights variable persists between
		// multiple calls to this function (it is essentially
		// a global variable that can only be accessed
		// in this function).
		static std::list<PtrType *> flyweights;
		for(PtrType * fly : flyweights){
			if (fly->myBasicType == basicType){
				if (fly->myLevel == level){
					return fly;
				}
			}
		}
		PtrType * newType = new PtrType(basicType, level);
		flyweights.push_back(newType);
		return newType;
	}

	std::string getString() const override{
		std::string res = myBasicType->getString();
		for (int i = 0 ; i < myLevel ; i++){
			res += "ptr";
		}
		return res;
	}

	/* Add a level of indirection from a pointer type */
	DataType * incLevel() const {
		return PtrType::produce(myBasicType, myLevel + 1);
	}

	/* Remove a level of indirection to the pointer type */
	DataType * decLevel() const{
		return PtrType::produce(myBasicType, myLevel - 1);
	}

	static DataType * derefType(const DataType * type){
		if (type->asError()){ 
			return ErrorType::produce();
		} else if (const PtrType * t = type->asPtr()){ 
			return t->decLevel(); 
		} else if (const BasicType * t = type->asBasic()){ 
			return nullptr;
		} else {
			return nullptr;
		}
	}

	static DataType * refType(const DataType * type){
		if (type->asError()){ 
			return ErrorType::produce();
		} else if (const PtrType * t = type->asPtr()){ 
			return t->incLevel(); 
		} else if (const BasicType * t = type->asBasic()){ 
			return PtrType::produce(t, 1);
		}
		return nullptr;
	}

	virtual bool validVarType() const override {
		return true;
	}
	bool isPtr() const override { return true; } 
	const PtrType * asPtr() const override { return this; }
	int getLevel(){ return myLevel; }
	
private:
	PtrType(const BasicType * basicType, int level)
	: myBasicType(basicType), myLevel(level){
		/* private constructor, can only be called from produce */
	}
	const BasicType * myBasicType;
	int myLevel;
};

//DataType subclass to represent the type of a function. It will
// have a list of argument types and a return type. 
class FnType : public DataType{
public:
	FnType(const std::list<const DataType *>* formalsIn, const DataType * retTypeIn) 
	: DataType(),
	  myFormalTypes(formalsIn),
	  myRetType(retTypeIn)
	{
	}
	std::string getString() const override{
		std::string result = "";
		bool first = true;
		for (auto elt : *myFormalTypes){
			if (first) { first = false; }
			else { result += ","; }
			result += elt->getString();
		}
		result += "->";
		result += myRetType->getString();
		return result;
	}
	virtual const FnType * asFn() const override { return this; }

	const DataType * getReturnType() const {
		return myRetType;
	}
	const std::list<const DataType *> * getFormalTypes() const {
		return myFormalTypes;
	}
	virtual bool validVarType() const override { return false; }
private:
	const std::list<const DataType *> * myFormalTypes;
	const DataType * myRetType;
};

}

#endif
