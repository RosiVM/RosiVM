#include "namescope.h"

using namespace std;
using namespace rvm;

Symbol* rvm::NameScope::selfLookup(string name) {
    auto kvp = _symbols.find(name);
    if (kvp == _symbols.end()) return nullptr;
    return kvp->second.get();
}

Symbol* rvm::NameScope::create(string name) {
    auto symbolPtr = selfLookup(name);
    if (symbolPtr != nullptr) return symbolPtr;

    auto symbol = std::make_unique<Symbol>(name);
    symbolPtr = symbol.get();
    _symbols[name] = std::move(symbol);
    return symbolPtr;
}

Symbol* rvm::NameScope::lookup(std::string name) {
    auto selfSymbol = selfLookup(name);
    if (selfSymbol != nullptr) return selfSymbol;
    if (_parent == nullptr) return nullptr;
    return _parent->lookup(name);
}

void rvm::NameScope::addSymbolDeclaration(std::string name, rvm::ast::ModuleMember* declaration) {
    create(name)->add(declaration);
}
