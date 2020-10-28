#include <list>
#include <sstream>

#include "types.hpp"
#include "ast.hpp"

namespace holeyc{

std::string BasicType::getString() const{
	std::string res = "";
	switch(myBaseType){
	case BaseType::INT:
		res += "int";
		break;
	case BaseType::BOOL:
		res += "bool";
		break;
	case BaseType::VOID:
		res += "void";
		break;
	case BaseType::CHAR:
		res += "char";
		break;
	}
	return res;
}

DataType * CharTypeNode::getType() { 
	if (isPtr){
		return PtrType::produce(BasicType::CHAR(), 1);
	} else {
		return BasicType::CHAR(); 
	}
}

DataType * BoolTypeNode::getType() { 
	if (isPtr){
		return PtrType::produce(BasicType::BOOL(), 1);
	} else {
		return BasicType::BOOL(); 
	}
}

DataType * IntTypeNode::getType() { 
	if (isPtr){
		return PtrType::produce(BasicType::INT(), 1);
	} else {
		return BasicType::INT(); 
	}
}


} //End namespace
