#ifndef RVM_TYPECHECKER_H
#define RVM_TYPECHECKER_H

#include <vector>

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

        std::vector<std::unique_ptr<rvm::type::Type>> _types;

    public:
        TypeChecker(Binder* binder) : _binder(binder), _currentScope(binder) {}

        /// Fully type check all members of the module.
        void check(rvm::Parser* module) {
            module->visit(this);
        }

        void on(rvm::ast::FunctionArgument* arg) {
            // Get the type of the type expression and move on the argument.
            // Normally vars and consts will either force type from type annotation or infer the type from assignment.
            // For function arguments the type can not be infered.
            arg->typeAnnotation()->visit(this);
            arg->setType(arg->typeAnnotation()->type());
        }

        void on(rvm::ast::FunctionPrototype* proto) {
            std::vector<rvm::type::Type*> argumentTypes;
            for(auto& arg : proto->args()) {
                on(arg.get());
                argumentTypes.push_back(arg->type());
            }

            proto->returnTypeAnnotation()->visit(this);
            rvm::type::Type* returnType = proto->returnTypeAnnotation()->type();

            auto type = std::make_unique<rvm::type::SignatureType>(returnType, std::move(argumentTypes));
            proto->setType(type.get());
            _types.push_back(std::move(type));
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
            auto& typeExpression = statement->typeAnnotation();
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
            // assert(false);
            exit(1);
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
                default:
                    exit(1);
                    // assert(false);
                    break;
            }
        }
        void on(rvm::ast::MemberAccessExpression* expression) override {
            // TODO:
            exit(1);
            // assert(false);
        }
        void on(rvm::ast::InvocationExpression* expression) override {
            expression->operand()->visit(this);
            auto functionType = expression->operand()->type();

            // TODO: This should force the Symbol to typecheck its functions and function declarations.
            // TODO: This should probably include "declaration + function signature", otherwise the emitter won't know the function name and how to name mangle for the linker. Calling convention?
            auto& overloads = functionType->callSignatures();
            if (overloads.size() == 0)
                exit(1);
                // assert(false); // TODO: CompilerError! Not callable...

            // TODO: Make union types for the argument values to provide as context when resolving values.
            std::vector<rvm::type::Type*> values;
            for (auto& value : expression->values()) {
                value->visit(this);
                values.push_back(value->type());
            }

            // Now having the values and their types, resolve an overload... or throw.
            for (auto& overload : overloads) {
                // TODO: Create ReferenceType... expression->operand()->setType(overload);
            }

            // TODO: expression->setType(overload->returnType());
            // assert(false);
            exit(1);
        }
        void on(rvm::ast::ConditionalIfExpression* expression) override {
            // TODO:
            // assert(false);
            exit(1);
        }
        void on(rvm::ast::UnaryExpression* expression) override {
            // TODO:
            exit(1);
            // assert(false);
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

                    // TODO: float + float = float
                    throw CompilerError(ErrorCode::BinaryExpressionTypeError, expression->span());
                }
                default: throw CompilerError(ErrorCode::BinaryExpressionTypeError, expression->span());
            }
        }
    };
};

#endif