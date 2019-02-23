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
#include "typechecker.h"

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

// TODO: Belong to LLIRCompiler...
static LLVMContext TheContext;
static IRBuilder<> Builder(TheContext);
static std::unique_ptr<Module> TheModule;

class LLIRCompiler : public ModuleMemberVisitor {
public:
    void on(rvm::ast::Function* f) override {
        cout << "compile " << f->name() << endl;

        // Make the function type:  double(double,double) etc.
        std::vector<Type*> Doubles(f->proto()->args().size(), Type::getFloatTy(TheContext));
        FunctionType *FT = FunctionType::get(Type::getFloatTy(TheContext), Doubles, false);
        llvm::Function *F = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, f->name(), TheModule.get());

        unsigned i = 0;
        for (auto &Arg : F->args()) {
            auto name = f->proto()->args()[i++]->name();
            Arg.setName(name);
        }
    }

    void on(rvm::ast::FunctionDeclaration* f) override {
        cout << "compile " << f->name() << endl;

        // Make the function type:  double(double,double) etc.
        std::vector<Type*> Doubles(f->proto()->args().size(), Type::getFloatTy(TheContext));
        FunctionType *FT = FunctionType::get(Type::getFloatTy(TheContext), Doubles, false);
        llvm::Function *F = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, f->name(), TheModule.get());

        unsigned i = 0;
        for (auto &Arg : F->args()) {
            auto name = f->proto()->args()[i++]->name();
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

    Parser module(program);
    module.parseModule();

    ASTPrinter printer;
    module.visit(&printer);

    Binder globalSymbols;
    module.visit(&globalSymbols);

    TypeChecker typeChecker(&globalSymbols);
    typeChecker.check(&module);

    // parser.visit(&typeChecker);

    TheModule = llvm::make_unique<Module>("my cool jit", TheContext);
    LLIRCompiler llirCompiler;
    module.visit(&llirCompiler);
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
