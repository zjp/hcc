#include "symbol_table.hpp"
namespace holeyc {

ScopeTable::ScopeTable() {
	symbols = new HashMap<std::string, SemSymbol*> ();
}

SemSymbol* ScopeTable::is_in_table(std::string name) {
	auto search = symbols->find(name);
	if(search != symbols->end()) {
		return search->second;
	}
	return nullptr;
}

void ScopeTable::insert(SemSymbol* symbol) {
	if(!is_in_table(symbol->getName())) {
		this->symbols->insert({symbol->getName(), symbol});
	}
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

void SymbolTable::insert(SemSymbol* symbol) {
	scopeTableChain->back()->insert(symbol);
}

SemSymbol* SymbolTable::lookup(std::string name) {
	SemSymbol* search = nullptr;
	for(auto it = scopeTableChain->begin(); it != scopeTableChain->end(); ++it) {
		search = (*it)->is_in_table(name);
	}
	return search;
}

}
