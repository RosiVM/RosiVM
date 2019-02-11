#include <vector>
#include <map>
#include <cassert>
#include <stack>

#include "source.h"
#include "lexer.h"
#include "parser.h"
#include "binder.h"
#include "print.h"
#include "types.h"

using namespace std;
using namespace rvm;
using namespace rvm::ast;

/// EXPLORATION TESTS ///

void printTokens(string program) {
    Lexer lexer(program);
    try {
        for (auto token : lexer) {
            if (token.type() != TokenType::Whitespace) {
                cout << token << endl;
            }
        }
    }
    catch(CompilerError e) {
        cout << endl << e.what() << endl;
    }
}

void testSimpleProgram1() {
    string program = "void main() {\r\n    console.log(\"Hello World!\");\r\n}"s;
    printTokens(program);
}

void testSimpleProgram2() {
    string program = "void main() {\r\n    var x = 15;\r\n var y = x / 2.0;\r\n    return y;\r\n}"s;
    printTokens(program);
}

void testSimpleProgram3() {
    string program = "void main() {\r\n    var x = 15e-2;\r\n var y = x << 3;\r\n    x %= y;\r\n    return x;\r\n}"s;
    printTokens(program);
}

// #include "llvm/ADT/APFloat.h"
// #include "llvm/ADT/STLExtras.h"
// #include "llvm/IR/BasicBlock.h"
// #include "llvm/IR/Constants.h"
// #include "llvm/IR/DerivedTypes.h"
// #include "llvm/IR/Function.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"

using namespace llvm;

// #include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"

class TypeChecker :
    public rvm::ast::ModuleMemberVisitor,
    public rvm::ast::TypeExpressionVisitor,
    public rvm::ast::StatementVisitor {

    Binder* _binder;
    NameScope* _currentScope;

public:
    TypeChecker(Binder* binder) : _binder(binder), _currentScope(binder) {}

    void on(rvm::ast::Function* f) override {
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
        // TODO:
        assert(false);
    }
    void on(rvm::ast::CodeBlock* statement) override {
        for(auto& statement : statement->statements())
            statement->visit(this);
    }
    void on(rvm::ast::ConstStatement* statement) override {
        // TODO: There should be a difference between type annotation and type resolved by the checker...
        ptr_type& typeExpression = statement->type();
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

// TODO: Belong to LLIRCompiler...
static LLVMContext TheContext;
static IRBuilder<> Builder(TheContext);
static std::unique_ptr<Module> TheModule;

class LLIRCompiler : public ModuleMemberVisitor {
public:
    void on(rvm::ast::Function* f) override {
        cout << "compile " << f->name() << endl;

        // Make the function type:  double(double,double) etc.
        std::vector<Type*> Doubles(f->args().size(), Type::getFloatTy(TheContext));
        FunctionType *FT = FunctionType::get(Type::getFloatTy(TheContext), Doubles, false);
        llvm::Function *F = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, f->name(), TheModule.get());

        unsigned i = 0;
        for (auto &Arg : F->args()) {
            auto name = f->args()[i++]->name();
            Arg.setName(name);
        }
    }

    void on(rvm::ast::FunctionDeclaration* f) override {
        cout << "compile " << f->name() << endl;

        // Make the function type:  double(double,double) etc.
        std::vector<Type*> Doubles(f->args().size(), Type::getFloatTy(TheContext));
        FunctionType *FT = FunctionType::get(Type::getFloatTy(TheContext), Doubles, false);
        llvm::Function *F = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, f->name(), TheModule.get());

        unsigned i = 0;
        for (auto &Arg : F->args()) {
            auto name = f->args()[i++]->name();
            Arg.setName(name);
        }
    }
};

void testSimpleProgramAST1() {
    string program = ""
        "declare function sin(angle: float): float;\r\n"s
        "declare function cos(angle: float): float;\r\n"s
        "function main(): float {\r\n"s
        "    const x = sin(15.0) + cos(15.0) * 2;"s
        "    const y = 2 * sin(15.0) + cos(15.0);"s
        "    return x;"s
        "}"s;

    Lexer lexer(program);
    printTokens(program);
    cout << "====================" << endl;

    Parser parser(program);
    parser.parseModule();

    ASTPrinter printer;
    parser.visit(&printer);

    Binder globalSymbols;
    parser.visit(&globalSymbols);

    TypeChecker typeChecker(&globalSymbols);
    parser.visit(&typeChecker);

    TheModule = llvm::make_unique<Module>("my cool jit", TheContext);
    LLIRCompiler llirCompiler;
    parser.visit(&llirCompiler);
    TheModule->print(errs(), nullptr);
}

int main(int argl, char** argv) {
    // cout << "testSimpleProgram1" << endl;
    // testSimpleProgram1();
    // cout << "testSimpleProgram2" << endl;
    // testSimpleProgram2();
    // cout << "testSimpleProgram3" << endl;
    // testSimpleProgram3();

    testSimpleProgramAST1();
}
