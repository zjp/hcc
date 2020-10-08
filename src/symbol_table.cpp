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
	scopeTableChain->push_front(new ScopeTable());
}

void SymbolTable::drop_scope() {
	scopeTableChain->pop_front();
}

void SymbolTable::insert(SemSymbol* symbol) {
	scopeTableChain->front()->insert(symbol);
}

SemSymbol* SymbolTable::lookup_any(std::string name) {
	SemSymbol* search = nullptr;
	for(auto it = scopeTableChain->begin(); it != scopeTableChain->end(); ++it) {
		SemSymbol* temp = (*it)->is_in_table(name);
		if(temp != nullptr) {
			search = temp;
			break;
		}
	}
	return search;
}

SemSymbol *SymbolTable::lookup_front(std::string name) {
	return (*scopeTableChain->begin())->is_in_table(name);
}

}
