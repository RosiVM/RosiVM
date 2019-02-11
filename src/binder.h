#ifndef RVM_BINDER_H
#define RVM_BINDER_H

#include <string>
#include <vector>
#include "namescope.h"
#include "ast.h"

namespace rvm {
    /// The Binder runs as a compile pass that generates the global NameScope.
    class Binder : public NameScope, public rvm::ast::ModuleMemberVisitor {
    public:
        Binder() {}
        void on(rvm::ast::Function* f) override { addSymbolDeclaration(f->name(), f); }
        void on(rvm::ast::FunctionDeclaration* f) override { addSymbolDeclaration(f->name(), f); }
    };
}

#endif
