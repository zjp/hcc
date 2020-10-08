#ifndef HOLEYC_SYMBOL_TABLE_HPP
#define HOLEYC_SYMBOL_TABLE_HPP
#include <string>
#include <unordered_map>
#include <list>

#include "errors.hpp"

//Use an alias template so that we can use
// "HashMap" and it means "std::unordered_map"
template <typename K, typename V>
using HashMap = std::unordered_map<K, V>;

using namespace std;

namespace holeyc {

//A semantic symbol, which represents a single
// variable, function, etc. Semantic symbols
// exist for the lifetime of a scope in the
// symbol table.
class SemSymbol {
public:

	//TODO add the fields that
	// each semantic symbol should track
	// (i.e. the kind of the symbol (either a variable or function)
	// and functions to get/set those fields
	SemSymbol(std::string decl_type, std::string type, std::string name)
		: decl_type(decl_type), type(type), name(name) {}

	std::string getDeclType() { return decl_type; }
	std::string getType() { return type; }
	std::string getName() { return name; }

	void setType(std::string type) { this->type = type; }
	void setDeclType(std::string decl_type) { this->decl_type = decl_type; }
	void setName(std::string name) { this->name = name; }
	virtual void unparse(std::ostream& out) = 0;

private:
	std::string decl_type;
	std::string type;
	std::string name;
};

class FnSymbol : public SemSymbol {
public:
	FnSymbol(std::string type, std::string name, std::list<std::string>* formals)
		: SemSymbol("func", type, name), formals(formals) {}
	void unparse(std::ostream &out) override;
private:
	std::list<std::string>* formals;
};

class VarSymbol : public SemSymbol {
public:
	VarSymbol(std::string type, std::string name)
		: SemSymbol("var", type, name) {}
        void unparse(std::ostream &out) override;
};
//A single scope. The symbol table is broken down into a
// chain of scope tables, and each scope table holds
// semantic symbols for a single scope. For example,
// the globals scope will be represented by a ScopeTable,
// and the contents of each function can be represented by
// a ScopeTable.
class ScopeTable {
public:
	ScopeTable();
	//TODO: add functions for looking up symbols
	// and/or returning information to indicate
	// that the symbol does not exist within the
	// current scope.
	SemSymbol* is_in_table(std::string name);

	void insert(SemSymbol* symbol);
private:
	HashMap<std::string, SemSymbol*>* symbols;
};

class SymbolTable {
public:
	SymbolTable();
	//TODO: add functions to create a new ScopeTable
	// when a new scope is entered, drop a ScopeTable
	// when a scope is exited, etc.

	// Allocate a new empty scope table
	void add_scope();

	// Remove all entries from the last scope and free it
	void drop_scope();

	// Insert into the last scope
	void insert(SemSymbol* symbol);

	void del();

	SemSymbol* lookup(std::string name);

	void set_attr();

	void get_attr();

	void errMultDef(size_t l, size_t c) {
		Report::fatal(l, c, "Multiply declared identifier");
	}

	void errUndec(size_t l, size_t c) {
		Report::warn(l, c, "Undeclared identifier");
	}

	void errBadTpe(size_t l, size_t c) {
		Report::fatal(l, c, "Invalid type in declaration");
	}

private:
	std::list<ScopeTable*>* scopeTableChain;
};

}

#endif
