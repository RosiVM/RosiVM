#ifndef RVM_SEMANTIC_H
#define RVM_SEMANTIC_H

#include <vector>
#include "ast.h"

namespace rvm {
    namespace sem {
        class Symbol {
            std::vector<rvm::ast::ModuleMember*> _declarations;
        public:
            Symbol() : _declarations() {}
        };
    };

    class Binder : public rvm::ast::ModuleMemberVisitor {
        std::map<std::string, rvm::sem::Symbol*> _globalSymbols;
    public:
        Binder() : _globalSymbols() {}

        // Accumulate declarations in the symbols table
        void on(rvm::ast::Function* f) override {
            // TODO: Add to scope, check for duplicates...
        }
        // Accumulate declarations in the symbols table
        void on(rvm::ast::FunctionDeclaration* f) override {
            // TODO: Add to scope, check for duplicates...
        }
    };
};

#endif