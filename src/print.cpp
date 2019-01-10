#include <iostream> 
#include "print.h"

using namespace std;
using namespace rvm;
using namespace rvm::ast;

void ASTPrinter::on(Function* f) {
    cout << "function " << f->name() << "(";
    bool firstArg = true;
    for (auto& arg : f->args()) {
        if (!firstArg) cout << ", ";
        cout << arg->name() << ": ";
        arg->type()->visit(this);
        firstArg = false;
    }
    cout << ")";

    if (f->returnType()) {
        cout << ": ";
        f->returnType()->visit(this);
    }

    f->codeBlock()->visit(this);
}

void ASTPrinter::on(FunctionDeclaration* f) {
    cout << "declare function " << f->name() << "(";
    bool firstArg = true;
    for (auto& arg : f->args()) {
        if (!firstArg) cout << ", ";
        cout << arg->name() << ": ";
        arg->type()->visit(this);
        firstArg = false;
    }
    cout << ")";
    if (f->returnType()) {
        cout << ": ";
        f->returnType()->visit(this);
    }
    cout << ";" << endl;
}

void ASTPrinter::on(PrimitiveTypeExpression* t) {
    switch(t->type()) {
        case PrimitiveType::Int:
            cout << "int";
            break;
        case PrimitiveType::Float:
            cout << "float";
            break;
        case PrimitiveType::String:
            cout << "string";
            break;
        case PrimitiveType::Bool:
            cout << "bool";
            break;
    }
}

// Statements
void ASTPrinter::on(CodeBlock* statement) {
    cout << " {" << endl;
    for (auto& statement : statement->statements()) {
        statement->visit(this);
    }
    cout << "}" << endl;
}

void ASTPrinter::on(ConstStatement* statement) {
    cout << "const " << statement->name();
    ptr_type& type = statement->type();
    if (type != nullptr) {
        cout << ": ";
        type->visit(this);
    }
    ptr_value& value = statement->value();
    cout << " = ";
    value->visit(this);
    cout << ";" << endl;
}

void ASTPrinter::on(ReturnStatement* statement) {
    cout << "return";
    ptr_value& value = statement->value();
    if (value != nullptr) {
        cout << " ";
        value->visit(this);
    }
    cout << ";" << endl;
}

// Value expressions
void ASTPrinter::on(IdentifierExpression* expression) {
    cout << expression->name();
}

void ASTPrinter::on(ConstantValueExpression* expression) {
    cout << expression->literal().code();
}

void ASTPrinter::on(MemberAccessExpression* expression) {
    cout << "?";
}

void ASTPrinter::on(InvocationExpression* expression) {
    expression->operand()->visit(this);
    cout << "(";
    bool isFirst = true;
    for(auto& value : expression->values()) {
        if (!isFirst) cout << ", ";
        isFirst = false;
        value->visit(this);
    }
    cout << ")";
}

void ASTPrinter::on(ConditionalIfExpression* expression) {
    cout << "?";
}

void ASTPrinter::on(UnaryExpression* expression) {
    switch(expression->op()) {
        case UnaryOperator::ConditionalNotOperator: cout << "!"s; break;
        case UnaryOperator::UnaryPlusOperator: cout << "+"s; break;
        case UnaryOperator::UnaryMinusOperator: cout << "-"s; break;
        case UnaryOperator::PreIncrementOperator: cout << "++"s; break;
        case UnaryOperator::PreDecrementOperator: cout << "--"s; break;
        case UnaryOperator::BitComplementOperator: cout << "~"s; break;
        default:;
    }

    expression->operand()->visit(this);

    switch(expression->op()) {
        case UnaryOperator::PostIncrementOperator: cout << "++"s; break;
        case UnaryOperator::PostDecrementOperator: cout << "--"s; break;
        default:;
    }
}

void ASTPrinter::on(BinaryExpression* expression) {
    expression->lhs()->visit(this);
    cout << " "s << toString(expression->op()) << " "s;
    expression->rhs()->visit(this);
}
