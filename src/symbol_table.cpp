#include "symbol_table.hpp"
namespace holeyc {

ScopeTable::ScopeTable() {
	symbols = new HashMap<std::string, SemSymbol*> ();
}

bool ScopeTable::is_in_table() {
	return true;
}

SymbolTable::SymbolTable(){
	//TODO: implement the list of hashtables approach
	// to building a symbol table:
	// Upon entry to a scope a new scope table will be
	// entered into the front of the chain and upon exit the
	// latest scope table will be removed from the front of
	// the chain.
	scopeTableChain = new std::list<ScopeTable*>();
}

void SymbolTable::add_scope() {
	scopeTableChain->push_back(new ScopeTable());
}

void SymbolTable::drop_scope() {
	scopeTableChain->pop_back();
}

}
