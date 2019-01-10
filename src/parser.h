#ifndef RVM_PARSER_H
#define RVM_PARSER_H

#include <string>
#include <vector>
#include "lexer.h"
#include "ast.h"

namespace rvm {
    class Parser {
        typedef typename rvm::Lexer::TokenIterator TokenIterator;
        typedef typename rvm::Lexer::Token Token;

        rvm::Lexer _lexer;
        TokenIterator _current;
        TokenIterator _end;
        Token _lookaheadToken;
        std::vector<std::unique_ptr<ast::ModuleMember> > _members;

    public:

        Parser(std::string code) :
            _lexer(Lexer(code)),
            _current(_lexer.begin()),
            _end(_lexer.end()),
            _lookaheadToken(*_current) {
        }

        void parseModule() { parseModuleMembers(); }
        std::vector<std::unique_ptr<ast::ModuleMember> >& members() { return _members; }

        void visit(ast::ModuleMemberVisitor* visitor) {
            for(auto& f : _members) f->visit(visitor);
        }

    private:
        inline SourceSpan span() { return _lookaheadToken.span(); }

        template<TokenType type>
        inline bool is() { return _lookaheadToken == type; }

        template<TokenType type>
        inline void expect() {
            if (!is<type>()) throw CompilerError(ErrorCode::UnexpectedToken, span());
        }

        template<TokenType type>
        inline Token consume() {
            expect<type>();
            return consumeToken();
        }

        inline Token consumeToken();
        ast::ptr_value parseValueExpression();
        ast::ptr_value parsePrec1ValueExpression();
        ast::ptr_value parsePrec2ValueExpression();
        ast::ptr_value parsePrec3ValueExpression();
        ast::ptr_value parsePrec4ValueExpression();
        ast::ptr_value parsePrec5ValueExpression();
        ast::ptr_value parsePrec6ValueExpression();
        ast::ptr_value parsePrec7ValueExpression();
        ast::ptr_value parsePrec8ValueExpression();
        ast::ptr_value parsePrec9ValueExpression();
        ast::ptr_value parsePrec10ValueExpression();
        ast::ptr_value parsePrec11ValueExpression();
        ast::ptr_value parsePrec12ValueExpression();
        ast::ptr_value parsePrec13ValueExpression();
        std::unique_ptr<ast::ConstStatement> parseConstStatement();
        std::unique_ptr<ast::ReturnStatement> parseReturnStatement();
        ast::ptr_statement parseStatement();
        std::unique_ptr<ast::CodeBlock> parseCodeBlock();
        ast::ptr_type parseTypeExpression();
        std::unique_ptr<ast::FunctionArgument> consumeFunctionArgument();
        std::unique_ptr<ast::FunctionPrototype> consumeFunctionPrototype();
        std::unique_ptr<ast::Function> consumeFunction();
        std::unique_ptr<ast::FunctionDeclaration> consumeFunctionDeclaration(Token& declareKeyword);
        void parseModuleMembers();
    };
};

#endif