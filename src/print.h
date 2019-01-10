#ifndef RVM_PRINT_H
#define RVM_PRINT_H

#include "ast.h"

namespace rvm {
    class ASTPrinter :
        public rvm::ast::ModuleMemberVisitor,
        public rvm::ast::TypeExpressionVisitor,
        public rvm::ast::StatementVisitor {
    public:
        void on(rvm::ast::Function* f) override;
        void on(rvm::ast::FunctionDeclaration* f) override;
        void on(rvm::ast::PrimitiveTypeExpression* t) override;
        void on(rvm::ast::CodeBlock* statement) override;
        void on(rvm::ast::ConstStatement* statement) override;
        void on(rvm::ast::ReturnStatement* statement) override;
        void on(rvm::ast::IdentifierExpression* expression) override;
        void on(rvm::ast::ConstantValueExpression* expression) override;
        void on(rvm::ast::MemberAccessExpression* expression) override;
        void on(rvm::ast::InvocationExpression* expression) override;
        void on(rvm::ast::ConditionalIfExpression* expression) override;
        void on(rvm::ast::UnaryExpression* expression) override;
        void on(rvm::ast::BinaryExpression* expression) override;
    };
};

#endif