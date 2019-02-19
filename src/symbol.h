#ifndef RVM_SYMBOL_H
#define RVM_SYMBOL_H

#include <string>
#include "types.h"
#include "ast.h"

namespace rvm {
    /// Symbols can be looked up in NameScopes by name,
    /// and represent a list of resolved declarations.
    class Symbol : public rvm::type::Type {
        std::string _name;
        std::vector<rvm::ast::ModuleMember*> _declarations;

    public:
        Symbol(std::string name) : _declarations(), _name(name) {}
        void add(rvm::ast::ModuleMember* declaration) { _declarations.push_back(declaration); }
    };
};

#endif