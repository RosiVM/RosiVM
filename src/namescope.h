#ifndef RVM_NAMESCOPE_H
#define RVM_NAMESCOPE_H

#include <string>
#include <vector>
#include "ast.h"
#include "types.h"
#include "symbol.h"

namespace rvm {
    class NameScope {
        NameScope* _parent;
        std::map<std::string, std::unique_ptr<Symbol> > _symbols;

        /// Looks up a Symbol by name in this NameScope.
        /// Does not search recursively the parent NameScope.
        Symbol* selfLookup(std::string name);

        /// Creates a Symbol in this NameScope for name.
        Symbol* create(std::string name);

    public:
        NameScope() : _parent(nullptr) {}
        NameScope(NameScope* parent) : _parent(parent) {}

        /// Looks up a Symbol by name in this NameScope and recursively up the parents chain.
        Symbol* lookup(std::string name);

        /// Add the declaration to a Symbol with name in this NameScope.
        void addSymbolDeclaration(std::string name, rvm::ast::ModuleMember* declaration);
    };
}

#endif
