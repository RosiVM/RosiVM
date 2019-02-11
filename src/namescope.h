#ifndef RVM_NAMESCOPE_H
#define RVM_NAMESCOPE_H

#include <string>
#include <vector>
#include "ast.h"
#include "types.h"
#include "namescope.h"

namespace rvm {
    /// Symbols can be looked up in NameScopes by name,
    /// and represent a list of resolved declarations.
    class Symbol : public rvm::type::Type {
        std::string _name;
        std::vector<rvm::ast::ModuleMember*> _declarations;
    public:
        Symbol(std::string name) : _declarations(), _name(name) {}
        void add(rvm::ast::ModuleMember* declaration) { _declarations.push_back(declaration); }

        // TODO: Override isInvocable and lookupOverload...
    };

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
