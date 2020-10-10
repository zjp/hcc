#ifndef HOLEYC_NAME_ERROR_REPORTING_HH
#define HOLEYC_NAME_ERROR_REPORTING_HH

#include "errors.hpp"

namespace holeyc{

class NameErr{
public:
static bool undeclID(size_t line, size_t col){
	Report::fatal(line, col, "Undeclared identifier");
	return false;
}
static bool badVarType(size_t line, size_t col){
	Report::fatal(line, col, "Invalid type in declaration");
	return false;
}
static bool multiDecl(size_t line, size_t col){
	Report::fatal(line, col, "Multiply declared identifier");
	return false;
}
};

} //End namespace holeyc

#endif
