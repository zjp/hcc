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
	BasicType * base = BasicType::CHAR();
	if (isPtr){
		return PtrType::produce(base, 1);
	} else {
		return base;
	}
}

DataType * BoolTypeNode::getType() { 
	BasicType * base = BasicType::BOOL();
	if (isPtr){
		return PtrType::produce(base, 1);
	} else {
		return base;
	}
}

DataType * IntTypeNode::getType() { 
	BasicType * base = BasicType::INT();
	if (isPtr){
		return PtrType::produce(base, 1);
	} else {
		return base;
	}
}


} //End namespace
