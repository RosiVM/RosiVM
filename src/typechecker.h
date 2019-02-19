#ifndef RVM_TYPECHECKER_H
#define RVM_TYPECHECKER_H

#include "parser.h"
#include "ast.h"
#include "types.h"
#include "namescope.h"
#include "binder.h"

namespace rvm {
    class TypeChecker :
        public rvm::ast::ModuleMemberVisitor,
        public rvm::ast::TypeExpressionVisitor,
        public rvm::ast::StatementVisitor {

        Binder* _binder;
        NameScope* _currentScope;

    public:
        TypeChecker(Binder* binder) : _binder(binder), _currentScope(binder) {}

        /// Type checks all members of the module.
        void check(rvm::Parser* module) {
            module->visit(this);
        }

        void on(rvm::ast::FunctionArgument* arg) {
            // Get the type of the type expression and move on the argument, this can not be infered.
            arg->type()->visit(this);
            arg->setType(arg->type()->type());
        }

        void on(rvm::ast::FunctionPrototype* proto) {
            for(auto& arg : proto->args()) on(arg.get());
            proto->returnType()->visit(this);

            

            // TODO: Create a type, how to handle lifetime?
            // proto->setType();
            // TODO: proto->setType() ... new FunctionPrototypeType...
        }

        void on(rvm::ast::Function* f) override {
            on(f->proto().get());

            // TODO: Read the args, the return type, assign a type to the function declaration...
            // f->args()
            // f->returnType();

            // TODO: Store the return type on stack to typecheck return expressions inside...
            // TODO: Push the args in name scope...
            f->codeBlock()->visit(this);
        }
        void on(rvm::ast::FunctionDeclaration* f) override {
            // TODO: Read the args, the return type, assign a type to the function declaration...
        }
        void on(rvm::ast::PrimitiveTypeExpression* t) override {
            switch (t->type()) {
                case rvm::ast::PrimitiveType::Float:
                    t->setType(rvm::type::getFloat());
                    return;
                case rvm::ast::PrimitiveType::Int:
                    t->setType(rvm::type::getInt());
                    return;
                case rvm::ast::PrimitiveType::Bool:
                    t->setType(rvm::type::getBool());
                    return;
                case rvm::ast::PrimitiveType::String:
                    t->setType(rvm::type::getString());
                    return;
            }
        }
        void on(rvm::ast::CodeBlock* statement) override {
            for(auto& statement : statement->statements())
                statement->visit(this);
        }
        void on(rvm::ast::ConstStatement* statement) override {
            // TODO: There should be a difference between type annotation and type resolved by the checker...
            rvm::ast::ptr_type& typeExpression = statement->type();
            if (typeExpression != nullptr)
                typeExpression->visit(this);

            // Assignment for the const expressions is mandatory.
            statement->value()->visit(this);

            // TODO: Read the type annotation if any
            // TODO: Read the assignment expression and make sure it is assignable to the annotation, or infer type from it.
            // TODO: Push a const in the code block
        }
        void on(rvm::ast::ReturnStatement* statement) override {
            // TODO:
            assert(false);
        }
        void on(rvm::ast::IdentifierExpression* expression) override {
            Symbol* symbol = _currentScope->lookup(expression->name());
            if (symbol == nullptr) throw CompilerError(ErrorCode::UnknownSymbolReference, expression->span());
            expression->setType(symbol);
        }
        void on(rvm::ast::ConstantValueExpression* expression) override {
            auto token = expression->literal();
            switch(token.type()) {
                case TokenType::Float: expression->setType(rvm::type::getFloat()); break;
                case TokenType::Integer: expression->setType(rvm::type::getInt()); break;
                case TokenType::SingleQuotesString: expression->setType(rvm::type::getString()); break;
                case TokenType::DoubleQuotesString: expression->setType(rvm::type::getString()); break;
                default: assert(false); break;
            }
        }
        void on(rvm::ast::MemberAccessExpression* expression) override {
            // TODO:
            assert(false);
        }
        void on(rvm::ast::InvocationExpression* expression) override {
            expression->operand()->visit(this);

            auto functionType = expression->operand()->type();

            // TODO: functionType here could be a Symbol of function and function declarations
            // typecheck the ptoto of these functions recursively but don't overdo it by looking in the implementations...

            if (!functionType->isInvocable())
                assert(false); // TODO: CompilerError!

            for(auto& arg : expression->values())
                arg->visit(this);
            
            // auto declaration = functionType->lookupOverload(expression->values());
            // if (declaration == nullptr)
            //     assert(false); // TODO: CompilerError!

            // TODO: Check if the arg types will resolve to an overload...
            // TODO: Store the expression type to be the return type of the overload
            
            assert(false);
        }
        void on(rvm::ast::ConditionalIfExpression* expression) override {
            // TODO:
            assert(false);
        }
        void on(rvm::ast::UnaryExpression* expression) override {
            // TODO:
            assert(false);
        }
        void on(rvm::ast::BinaryExpression* expression) override {
            // TODO:
            switch(expression->op()) {
                case rvm::ast::BinaryOperator::AddOperator: {
                    // TODO: Maybe this should be for all + - * / operators, but what about = += -= etc?
                    expression->lhs()->visit(this);
                    expression->rhs()->visit(this);
                    auto lType = expression->lhs()->type();
                    auto rType = expression->rhs()->type();

                    // float + float = float
                    throw CompilerError(ErrorCode::BinaryExpressionTypeError, expression->span());
                }
                default: throw CompilerError(ErrorCode::BinaryExpressionTypeError, expression->span());
            }
        }
    };
};

#endif