#include <vector>
#include <map>
#include <cassert>

#include "lex.h"

using namespace std;
using namespace rvm;

/// PARSER ///

class Parser {
public:

    typedef typename Lexer::TokenIterator TokenIterator;
    typedef typename Lexer::Token Token;

    class ModuleMember;

    class Function;
    class FunctionDeclaration;
    class FunctionPrototype;
    class FunctionArgument;

    class Statement;
    class CodeBlock;
    class ConstStatement;
    class ReturnStatement;

    class ValueExpression;

    const static string UnaryOperatorString[8];
    enum UnaryOperator {
        // Precedence 12
        ConditionalNotOperator,
        UnaryPlusOperator,
        UnaryMinusOperator,
        PreIncrementOperator,
        PreDecrementOperator,
        BitComplementOperator,

        // Precedence 13
        PostIncrementOperator,
        PostDecrementOperator,
    };

    class UnaryExpression;
    enum BinaryOperator {
        AssignmentOperator,
        AdditionAssignmentOperator,
        SubtractionAssignmentOperator,
        MultiplicationAssignmentOperator,
        DivisionAssignmentOperator,
        BitAndAssignmentOperator,
        BitXOrAssignmentOperator,
        BitOrAssignmentOperator,
        ReminderAssignmentOperator,
        LeftShiftAssignmentOperator,
        RightShiftAssignmentOperator,
        // Precedence 2
        ConditionalOrOperator,
        // Precedence 3
        ConditionalAndOperator,
        // Precedence 4
        BitwiseOrOperator,
        // Precedence 5
        BitwiseXOrOperator,
        // Precedence 6
        BitwiseAndOperator,
        // Precedence 7
        EqualOperator,
        NotEqualOperator,
        // Precedence 8
        LessThanOperator,
        GreaterThanOperator,
        LessOrEqualOperator,
        GreaterOrEqualOperator,
        // Precedence 9
        LeftShiftOperator,
        RightShiftOperator,
        // Precedence 10
        AddOperator,
        SubtractOperator,
        // Precedence 11
        MultiplyOperator,
        DivideOperator,
        ReminderOperator,
    };
    const static string BinaryOperatorString[29];

    static const string toString(UnaryOperator op) { return UnaryOperatorString[op]; }
    static const string toString(BinaryOperator op) { return BinaryOperatorString[op]; }

    class BinaryExpression;

    // Precedence 0
    class AssignmentExpression;
    class AdditionAssignmentExpression;
    class SubtractionAssignmentExpression;
    class MultiplicationAssignmentExpression;
    class DivisionAssignmentExpression;
    class BitAndAssignmentExpression;
    class BitXOrAssignmentExpression;
    class BitOrAssignmentExpression;
    class ReminderAssignmentExpression;
    class LeftShiftAssignmentExpression;
    class RightShiftAssignmentExpression;

    // Precedence 1
    class ConditionalIfExpression;

    // Precedence 2
    class ConditionalOrExpression;

    // Precedence 3
    class ConditionalAndExpression;

    // Precedence 4
    class BitwiseOrExpression;

    // Precedence 5
    class BitwiseXOrExpression;

    // Precedence 6
    class BitwiseAndExpression;

    // Precedence 7
    class EqualExpression;
    class NotEqualExpression;

    // Precedence 8
    class LessThanExpression;
    class GreaterThanExpression;
    class LessOrEqualExpression;
    class GreaterOrEqualExpression;

    // Precedence 9
    class LeftShiftExpression;
    class RightShiftExpression;

    // Precedence 10
    class AddExpression;
    class SubtractExpression;

    // Precedence 11
    class MultiplyExpression;
    class DivideExpression;
    class ReminderExpression;

    // Precedence 12
    class ConditionalNotExpression;
    class UnaryPlusExpression;
    class UnaryMinusExpression;
    class PreIncrementExpression;
    class PreDecrementExpression;
    class BitComplementExpression;

    // Precedence 13
    class IdentifierExpression;
    class ConstantValueExpression;
    class MemberAccessExpression;
    class InvocationExpression;
    class PostIncrementExpression;
    class PostDecrementExpression;

    class TypeExpression;
    class PrimitiveTypeExpression;

    class ModuleMemberVisitor;
    class TypeExpressionVisitor;
    class StatementVisitor;

    typedef unique_ptr<Statement> ptr_statement;
    typedef unique_ptr<TypeExpression> ptr_type;
    typedef unique_ptr<ValueExpression> ptr_value;

private:

    Lexer _lexer;
    TokenIterator _current;
    TokenIterator _end;

    Token _lookaheadToken;

    vector<unique_ptr<ModuleMember> > _members;

public:

    class ModuleMemberVisitor {
    public:
        virtual void on(Function* f) = 0;
        virtual void on(FunctionDeclaration* f) = 0;
    };

    class TypeExpressionVisitor {
    public:
        virtual void on(PrimitiveTypeExpression* t) = 0;
    };

    class StatementVisitor {
    public:
        virtual void on(CodeBlock* block) {}
        virtual void on(ConstStatement* block) {}
        virtual void on(ReturnStatement* block) {}

        virtual void on(IdentifierExpression* expression) {}
        virtual void on(ConstantValueExpression* expression) {}
        virtual void on(MemberAccessExpression* expression) {}
        virtual void on(InvocationExpression* expression) {}
        virtual void on(ConditionalIfExpression* expression) {}

        virtual void on(UnaryExpression* expression) {}
        virtual void on(BinaryExpression* expression) {}
    };

    class ModuleMember {
    public:
        virtual void visit(ModuleMemberVisitor* visitor) = 0;
        virtual ~ModuleMember() {}
    };

    class Function : public ModuleMember {
        Token _identifier;
        unique_ptr<FunctionPrototype> _proto;
        unique_ptr<CodeBlock> _block;
    public:
        Function(Token identifier, unique_ptr<FunctionPrototype> proto, unique_ptr<CodeBlock> block) :
            _identifier(identifier),
            _proto(move(proto)),
            _block(move(block)) {}

        string name() { return _identifier.value<string>(); }
        vector<unique_ptr<FunctionArgument> >& args() { return _proto->args(); }
        ptr_type& returnType() { return _proto->returnType(); }
        unique_ptr<FunctionPrototype>& proto() { return _proto; }
        unique_ptr<CodeBlock>& codeBlock() { return _block; }

        void visit(ModuleMemberVisitor* visitor) override { visitor->on(this); }
    };

    class FunctionDeclaration : public ModuleMember {
        Token _identifier;
        unique_ptr<FunctionPrototype> _proto;
    public:
        FunctionDeclaration(Token identifier, unique_ptr<FunctionPrototype> proto) :
            _identifier(identifier),
            _proto(move(proto)) {}

        string name() { return _identifier.value<string>(); }
        vector<unique_ptr<FunctionArgument> >& args() { return _proto->args(); }
        ptr_type& returnType() { return _proto->returnType(); }
        unique_ptr<FunctionPrototype>& proto() { return _proto; }

        void visit(ModuleMemberVisitor* visitor) override { visitor->on(this); }
    };

    class FunctionArgument {
        Token _identifier;
        ptr_type _type;
    public:
        FunctionArgument(Token identifier, ptr_type type) : _identifier(identifier), _type(move(type)) {}

        string name() { return _identifier.value<string>(); }
        ptr_type& type() { return _type; }
    };

    class FunctionPrototype {
        vector<unique_ptr<FunctionArgument> > _args;
        ptr_type _returnType;
    public:
        FunctionPrototype(vector<unique_ptr<FunctionArgument> > args, ptr_type returnType) :
            _args(move(args)),
            _returnType(move(returnType)) {}
        
        vector<unique_ptr<FunctionArgument> >& args() { return _args; }
        ptr_type& returnType() { return _returnType; }
    };

    class TypeExpression {
    public:
        virtual void visit(TypeExpressionVisitor* visitor) = 0;
        virtual ~TypeExpression() {}
    };

    enum PrimitiveType {
        Int,
        Float,
        String,
        Bool,
    };

    class PrimitiveTypeExpression : public TypeExpression {
        Token _keyword;
        PrimitiveType _type;
    public:
        PrimitiveTypeExpression(Token keyword, PrimitiveType type) : _keyword(keyword), _type(type) {}
        PrimitiveType type() { return _type; }
        void visit(TypeExpressionVisitor* visitor) override {
            visitor->on(this);
        }
    };

    class Statement {
    public:
        virtual ~Statement() {}
        virtual void visit(StatementVisitor* visitor) = 0;
    };

    class CodeBlock : public Statement {
        vector<ptr_statement> _statements;
    public:
        CodeBlock(vector<ptr_statement> statements) : _statements(move(statements)) {}
        vector<ptr_statement>& statements() { return _statements; }
        void visit(StatementVisitor* visitor) override { visitor->on(this); }

    };

    class ConstStatement : public Statement {
        Token _identifier;
        ptr_type _type;
        ptr_value _value;
    public:
        ConstStatement(Token identifier, ptr_type type, ptr_value value) :
            _identifier(identifier),
            _type(move(type)),
            _value(move(value)) {}

        string name() { return _identifier.value<string>(); }
        ptr_type& type() { return _type; }
        ptr_value& value() { return _value; }
        void visit(StatementVisitor* visitor) override { visitor->on(this); }
    };

    class ReturnStatement : public Statement {
        ptr_value _value;
    public:
        ReturnStatement(ptr_value value) : _value(move(value)) {}
        ptr_value& value() { return _value; }
        void visit(StatementVisitor* visitor) override { visitor->on(this); }
    };

    class ValueExpression : public Statement {
    public:
        virtual ushort precedence() = 0;
    };

    class UnaryExpression : public ValueExpression {
        ptr_value _operand;
    public:
        UnaryExpression(ptr_value operand) : _operand(move(operand)) {}
        virtual UnaryOperator op() = 0;
        ptr_value& operand() { return _operand; }
    };

    class BinaryExpression : public ValueExpression {
        ptr_value _lhs, _rhs;
    public:
        BinaryExpression(ptr_value lhs, ptr_value rhs) : _lhs(move(lhs)), _rhs(move(rhs)) {}
        virtual BinaryOperator op() = 0;
        ptr_value& lhs() { return _lhs; }
        ptr_value& rhs() { return _rhs; }
    };

    #define UNARY_EXPRESSION_CLASS(CLASS, OPERATOR, PRECEDENCE)\
    class CLASS : public UnaryExpression {\
    public:\
        CLASS(ptr_value operand) : UnaryExpression(move(operand)) {}\
        ushort precedence() override { return PRECEDENCE; }\
        UnaryOperator op() override { return OPERATOR; }\
        void visit(StatementVisitor* visitor) override { visitor->on(this); }\
    };\

    #define BINARY_EXPRESSION_CLASS(CLASS, OPERATOR, PRECEDENCE)\
    class CLASS : public BinaryExpression {\
    public:\
        CLASS(ptr_value lhs, ptr_value rhs) : BinaryExpression(move(lhs), move(rhs)) {}\
        ushort precedence() override { return PRECEDENCE; }\
        BinaryOperator op() override { return OPERATOR; }\
        void visit(StatementVisitor* visitor) override { visitor->on(this); }\
    };\

    // Precedence 0
    BINARY_EXPRESSION_CLASS(AssignmentExpression, AssignmentOperator, 0)
    BINARY_EXPRESSION_CLASS(AdditionAssignmentExpression, AdditionAssignmentOperator, 0)
    BINARY_EXPRESSION_CLASS(SubtractionAssignmentExpression, SubtractionAssignmentOperator, 0)
    BINARY_EXPRESSION_CLASS(MultiplicationAssignmentExpression, MultiplicationAssignmentOperator, 0)
    BINARY_EXPRESSION_CLASS(DivisionAssignmentExpression, DivisionAssignmentOperator, 0)
    BINARY_EXPRESSION_CLASS(BitAndAssignmentExpression, BitAndAssignmentOperator, 0)
    BINARY_EXPRESSION_CLASS(BitXOrAssignmentExpression, BitXOrAssignmentOperator, 0)
    BINARY_EXPRESSION_CLASS(BitOrAssignmentExpression, BitOrAssignmentOperator, 0)
    BINARY_EXPRESSION_CLASS(ReminderAssignmentExpression, ReminderAssignmentOperator, 0)
    BINARY_EXPRESSION_CLASS(LeftShiftAssignmentExpression, LeftShiftAssignmentOperator, 0)
    BINARY_EXPRESSION_CLASS(RightShiftAssignmentExpression, RightShiftAssignmentOperator, 0)

    // Precedence 1
    class ConditionalIfExpression : public ValueExpression {
        ptr_value _ifExp, _thenExp, _elseExp;
    public:
        ConditionalIfExpression(ptr_value ifExp, ptr_value thenExp, ptr_value elseExp) :
            _ifExp(move(ifExp)),
            _thenExp(move(thenExp)),
            _elseExp(move(elseExp)) {}

        ptr_value& ifExpression() { return _ifExp; }
        ptr_value& thenExpression() { return _thenExp; }
        ptr_value& elseExpression() { return _elseExp; }

        ushort precedence() override { return 1; }
        void visit(StatementVisitor* visitor) override { visitor->on(this); }
    };

    // Precedence 2
    BINARY_EXPRESSION_CLASS(ConditionalOrExpression, ConditionalOrOperator, 2)

    // Precedence 3
    BINARY_EXPRESSION_CLASS(ConditionalAndExpression, ConditionalAndOperator, 3)

    // Precedence 4
    BINARY_EXPRESSION_CLASS(BitwiseOrExpression, BitwiseOrOperator, 4)

    // Precedence 5
    BINARY_EXPRESSION_CLASS(BitwiseXOrExpression, BitwiseXOrOperator, 5)

    // Precedence 6
    BINARY_EXPRESSION_CLASS(BitwiseAndExpression, BitwiseAndOperator, 6)

    // Precedence 7
    BINARY_EXPRESSION_CLASS(EqualExpression, EqualOperator, 7)
    BINARY_EXPRESSION_CLASS(NotEqualExpression, NotEqualOperator, 7)

    // Precedence 8
    BINARY_EXPRESSION_CLASS(LessThanExpression, LessThanOperator, 8)
    BINARY_EXPRESSION_CLASS(GreaterThanExpression, GreaterThanOperator, 8)
    BINARY_EXPRESSION_CLASS(LessOrEqualExpression, LessOrEqualOperator, 8)
    BINARY_EXPRESSION_CLASS(GreaterOrEqualExpression, GreaterOrEqualOperator, 8)

    // Precedence 9
    BINARY_EXPRESSION_CLASS(LeftShiftExpression, LeftShiftOperator, 9)
    BINARY_EXPRESSION_CLASS(RightShiftExpression, RightShiftOperator, 9)
    
    // Precedence 10
    BINARY_EXPRESSION_CLASS(AddExpression, AddOperator, 10)
    BINARY_EXPRESSION_CLASS(SubtractExpression, SubtractOperator, 10)

    // Precedence 11
    BINARY_EXPRESSION_CLASS(MultiplyExpression, MultiplyOperator, 11)
    BINARY_EXPRESSION_CLASS(DivideExpression, DivideOperator, 11)
    BINARY_EXPRESSION_CLASS(ReminderExpression, ReminderOperator, 11)

    // Precedence 12
    UNARY_EXPRESSION_CLASS(ConditionalNotExpression, ConditionalNotOperator, 12);
    UNARY_EXPRESSION_CLASS(UnaryPlusExpression, UnaryPlusOperator, 12);
    UNARY_EXPRESSION_CLASS(UnaryMinusExpression, UnaryMinusOperator, 12);
    UNARY_EXPRESSION_CLASS(PreIncrementExpression, PreIncrementOperator, 12);
    UNARY_EXPRESSION_CLASS(PreDecrementExpression, PreDecrementOperator, 12);
    UNARY_EXPRESSION_CLASS(BitComplementExpression, BitComplementOperator, 12);

    // Precedence 13 Expressions
    class IdentifierExpression : public ValueExpression {
        Token _identifier;
    public:
        IdentifierExpression(Token identifier) : _identifier(identifier) {}
        string name() { return _identifier.value<string>(); }
        ushort precedence() override { return 13; }
        void visit(StatementVisitor* visitor) override { visitor->on(this); }
    };
    class ConstantValueExpression : public ValueExpression {
        Token _literal;
    public:
        ConstantValueExpression(Token literal) : _literal(literal) {}
        ushort precedence() override { return 13; }
        Token literal() { return _literal; }
        void visit(StatementVisitor* visitor) override { visitor->on(this); }
    };
    class MemberAccessExpression : public ValueExpression {
        Token _name;
        ptr_value _operand;
    public:
        MemberAccessExpression(ptr_value operand, Token name) : _operand(move(operand)), _name(name) {}
        ptr_value& operand() { return _operand; }
        string name() { return _name.value<string>(); }
        ushort precedence() override { return 13; }
        void visit(StatementVisitor* visitor) override { visitor->on(this); }
    };
    class InvocationExpression : public ValueExpression {
        vector<ptr_value> _values;
        ptr_value _operand;
    public:
        InvocationExpression(ptr_value lhs, vector<ptr_value> values) : _operand(move(lhs)), _values(move(values)) {}
        ptr_value& operand() { return _operand; }
        ushort precedence() override { return 13; }
        vector<ptr_value>& values() { return _values; }
        void visit(StatementVisitor* visitor) override { visitor->on(this); }
    };

    UNARY_EXPRESSION_CLASS(PostIncrementExpression, PostIncrementOperator, 13);
    UNARY_EXPRESSION_CLASS(PostDecrementExpression, PostDecrementOperator, 13);

    Parser(string code) :
        _lexer(Lexer(code)),
        _current(_lexer.begin()),
        _end(_lexer.end()),
        _lookaheadToken(*_current) {
    }

    void parseModule() { parseModuleMembers(); }
    vector<unique_ptr<ModuleMember> >& members() { return _members; }

    void visit(ModuleMemberVisitor* visitor) {
        for(auto& f : _members) f->visit(visitor);
    }

private:
    SourceSpan span() { return _lookaheadToken.span(); }

    template<TokenType type>
    bool is() { return _lookaheadToken == type; }

    template<TokenType type>
    Token consume() {
        if (!is<type>()) throw CompilerError(ErrorCode::UnexpectedToken, span());
        return consumeToken();
    }

    template<TokenType type>
    void expect() {
        if (!is<type>()) throw CompilerError(ErrorCode::UnexpectedToken, span());
    }

    Token consumeToken() {
        Token token = _lookaheadToken;
        if (!is<TokenType::EoF>()) {
            _lookaheadToken = *++_current;
            // Ignore whitespace.
            // TODO: Capture last comment before members for docs.
            while(is<TokenType::Whitespace>()) _lookaheadToken = *++_current;
        }
        return token;
    }

    ptr_value parseValueExpression() {
        // ! Expressions
        // <Expression> ::= <Prec0Exp> | <Prec1Exp>

        // ! Assignment Operators
        // <Prec0Exp> ::= <Assign>
        //             | <AddAssign>
        //             | <SubtractAssign>
        //             | <MultiplyAssign>
        //             | <DivideAssign>
        //             | <BitAndAssign>
        //             | <BitXorAssign>
        //             | <BitOrAssign>
        //             | <ReminderAssign>
        //             | <ShiftLeftAssign>
        //             | <ShiftRightAssign>

        ptr_value lhs = parsePrec1ValueExpression();
        if (lhs->precedence() >= 12) {
            if (is<TokenType::Assignment>()) {
                // <Assign> ::= <Prec12Exp> assign <Expression>
                consume<TokenType::Assignment>();
                lhs = std::make_unique<AssignmentExpression>(move(lhs), parseValueExpression());
            } else if (is<TokenType::AdditionAssignment>()) {
                // <AddAssign> ::= <Prec12Exp> add-assign <Expression>
                consume<TokenType::AdditionAssignment>();
                lhs = std::make_unique<AdditionAssignmentExpression>(move(lhs), parseValueExpression());
            } else if (is<TokenType::SubtractionAssignment>()) {
                // <SubtractAssign> ::= <Prec12Exp> subtract-assign <Expression>
                consume<TokenType::SubtractionAssignment>();
                lhs = std::make_unique<SubtractionAssignmentExpression>(move(lhs), parseValueExpression());
            } else if (is<TokenType::MultiplicationAssignment>()) {
                // <MultiplyAssign> ::= <Prec12Exp> multiply-assign <Expression>
                consume<TokenType::MultiplicationAssignment>();
                lhs = std::make_unique<MultiplicationAssignmentExpression>(move(lhs), parseValueExpression());
            } else if (is<TokenType::DivisionAssignment>()) {
                // <DivideAssign> ::= <Prec12Exp> divide-assign <Expression>
                consume<TokenType::DivisionAssignment>();
                lhs = std::make_unique<DivisionAssignmentExpression>(move(lhs), parseValueExpression());
            } else if (is<TokenType::BitAndAssignment>()) {
                // <BitAndAssign> ::= <Prec12Exp> bit-and-assign <Expression>
                consume<TokenType::BitAndAssignment>();
                lhs = std::make_unique<BitAndAssignmentExpression>(move(lhs), parseValueExpression());
            } else if (is<TokenType::BitXOrAssignment>()) {
                // <BitXorAssign> ::= <Prec12Exp> bit-xor-assign <Expression>
                consume<TokenType::BitXOrAssignment>();
                lhs = std::make_unique<BitXOrAssignmentExpression>(move(lhs), parseValueExpression());
            } else if (is<TokenType::BitOrAssignment>()) {
                // <BitOrAssign> ::= <Prec12Exp> bit-or-assign <Expression>
                consume<TokenType::BitOrAssignment>();
                lhs = std::make_unique<BitOrAssignmentExpression>(move(lhs), parseValueExpression());
            } else if (is<TokenType::ReminderAssignment>()) {
                // <ReminderAssign> ::= <Prec12Exp> reminder-assign <Expression>
                consume<TokenType::ReminderAssignment>();
                lhs = std::make_unique<ReminderAssignmentExpression>(move(lhs), parseValueExpression());
            } else if (is<TokenType::LeftShiftAssignment>()) {
                // <ShiftLeftAssign> ::= <Prec12Exp> shift-left-assign <Expression>
                consume<TokenType::LeftShiftAssignment>();
                lhs = std::make_unique<LeftShiftAssignmentExpression>(move(lhs), parseValueExpression());
            } else if (is<TokenType::RightShiftAssignment>()) {
                // <ShiftRightAssign> ::= <Prec12Exp> shift-right-assign <Expression>
                consume<TokenType::RightShiftAssignment>();
                lhs = std::make_unique<RightShiftAssignmentExpression>(move(lhs), parseValueExpression());
            }
        }
        return lhs;
    }

    ptr_value parsePrec1ValueExpression() {
        // ! Conditional If Operators
        // <Prec1Exp> ::= <ConditionalIf> | <Prec2Exp>
        // <ConditionalIf> ::= <Prec2Exp> question <Expression> colon <Expression>
        ptr_value conditionExpression = parsePrec2ValueExpression();
        if (!is<TokenType::Question>()) return conditionExpression;

        consume<TokenType::Question>();
        ptr_value thenExpression = parseValueExpression();
        consume<TokenType::Colon>();
        ptr_value elseExpression = parseValueExpression();
        return std::make_unique<ConditionalIfExpression>(move(conditionExpression), move(thenExpression), move(elseExpression));
    }

    ptr_value parsePrec2ValueExpression() {
        // ! Conditional Operators
        // <Prec2Exp> ::= <Prec3Exp> | <ConditionalOr>
        // <ConditionalOr> ::= <Prec2Exp> conditional-or <Prec3Exp>       
        ptr_value lhs = parsePrec3ValueExpression();
        while(is<TokenType::ConditionalOr>()) {
            consume<TokenType::ConditionalOr>();
            ptr_value rhs = parsePrec3ValueExpression();
            lhs = std::make_unique<ConditionalOrExpression>(move(lhs), move(rhs));
        }
        return lhs;
    }

    ptr_value parsePrec3ValueExpression() {
        // ! Conditional Operators
        // <Prec3Exp> ::= <Prec4Exp> | <ConditionalAnd>
        // <ConditionalAnd> ::=  <Prec3Exp> conditional-and <Prec4Exp>
        ptr_value lhs = parsePrec4ValueExpression();
        while(is<TokenType::ConditionalAnd>()) {
            consume<TokenType::ConditionalAnd>();
            ptr_value rhs = parsePrec4ValueExpression();
            lhs = std::make_unique<ConditionalAndExpression>(move(lhs), move(rhs));
        }
        return lhs;
    }

    ptr_value parsePrec4ValueExpression() {
        // ! Bitwise Operators
        // <Prec4Exp> ::= <Prec5Exp> | <BitwiseOr>
        // <BitwiseOr> ::= <Prec4Exp> bitwise-or <Prec5Exp>
        ptr_value lhs = parsePrec5ValueExpression();
        while(is<TokenType::BitwiseOr>()) {
            consume<TokenType::BitwiseOr>();
            ptr_value rhs = parsePrec5ValueExpression();
            lhs = std::make_unique<BitwiseOrExpression>(move(lhs), move(rhs));
        }
        return lhs;
    }

    ptr_value parsePrec5ValueExpression() {
        // <Prec5Exp> ::= <Prec6Exp> | <BitwiseXOr>
        // <BitwiseXOr> ::= <Prec5Exp> bitwise-xor <Prec6Exp>
        ptr_value lhs = parsePrec6ValueExpression();
        while(is<TokenType::BitwiseXOr>()) {
            consume<TokenType::BitwiseXOr>();
            ptr_value rhs = parsePrec6ValueExpression();
            lhs = std::make_unique<BitwiseXOrExpression>(move(lhs), move(rhs));
        }
        return lhs;
    }

    ptr_value parsePrec6ValueExpression() {
        // <Prec6Exp> ::= <Prec7Exp> | <BitwiseAnd>
        // <BitwiseAnd> ::= <Prec6Exp> bitwise-and <Prec7Exp>
        ptr_value lhs = parsePrec7ValueExpression();
        while(is<TokenType::BitwiseAnd>()) {
            consume<TokenType::BitwiseAnd>();
            ptr_value rhs = parsePrec7ValueExpression();
            lhs = std::make_unique<BitwiseAndExpression>(move(lhs), move(rhs));
        }
        return lhs;
    }

    ptr_value parsePrec7ValueExpression() {
        // ! Equality Operators
        // <Prec7Exp> ::= <Prec8Exp> | <Equal> | <NotEqual>
        ptr_value lhs = parsePrec8ValueExpression();
        do {
            if (is<TokenType::Equal>()) {
                // <Equal> ::= <Prec7Exp> equal <Prec8Exp>
                consume<TokenType::Equal>();
                ptr_value rhs = parsePrec8ValueExpression();
                lhs = std::make_unique<EqualExpression>(move(lhs), move(rhs));
            } else if (is<TokenType::NotEqual>()) {
                // <NotEqual> ::= <Prec7Exp> not-equal <Prec8Exp>
                consume<TokenType::NotEqual>();
                ptr_value rhs = parsePrec8ValueExpression();
                lhs = std::make_unique<NotEqualExpression>(move(lhs), move(rhs));
            } else break;
        } while(true);
        return lhs;
    }

    ptr_value parsePrec8ValueExpression() {
        // ! Comparison Operators
        // <Prec8Exp> ::= <Prec9Exp> | <LessThan> | <GreaterThan> | <LessOrEqual> | <GreaterOrEqual>
        ptr_value lhs = parsePrec9ValueExpression();
        do {
            if (is<TokenType::Less>()) {
                // <LessThan> ::= <Prec8Exp> less-than <Prec9Exp>
                consume<TokenType::Less>();
                ptr_value rhs = parsePrec9ValueExpression();
                lhs = std::make_unique<LessThanExpression>(move(lhs), move(rhs));
            } else if (is<TokenType::Greater>()) {
                // <GreaterThan> ::= <Prec8Exp> greater-than <Prec9Exp>
                consume<TokenType::Greater>();
                ptr_value rhs = parsePrec9ValueExpression();
                lhs = std::make_unique<GreaterThanExpression>(move(lhs), move(rhs));
            } else if (is<TokenType::LessOrEqual>()) {
                // <LessOrEqual> ::= <Prec8Exp> less-or-equal <Prec9Exp>
                consume<TokenType::LessOrEqual>();
                ptr_value rhs = parsePrec9ValueExpression();
                lhs = std::make_unique<LessOrEqualExpression>(move(lhs), move(rhs));
            } else if (is<TokenType::GreaterOrEqual>()) {
                // <GreaterOrEqual> ::= <Prec8Exp> greater-or-equal <Prec9Exp>
                consume<TokenType::GreaterOrEqual>();
                ptr_value rhs = parsePrec9ValueExpression();
                lhs = std::make_unique<GreaterOrEqualExpression>(move(lhs), move(rhs));
            } else break;
        } while(true);
        return lhs;
    }

    ptr_value parsePrec9ValueExpression() {
        // ! Shift Operators
        // <Prec9Exp> ::= <Prec10Exp> | <LeftShift> | <RightShift>
        ptr_value lhs = parsePrec10ValueExpression();

        do {
            if (is<TokenType::LeftShift>()) {
                // <LeftShift> ::= <Prec9Exp> shift-left <Prec10Exp>
                consume<TokenType::LeftShift>();
                ptr_value rhs = parsePrec10ValueExpression();
                lhs = std::make_unique<LeftShiftExpression>(move(lhs), move(rhs));
            } else if (is<TokenType::RightShift>()) {
                // <RightShift> ::= <Prec9Exp> shift-right <Prec10Exp>
                consume<TokenType::RightShift>();
                ptr_value rhs = parsePrec10ValueExpression();
                lhs = std::make_unique<RightShiftExpression>(move(lhs), move(rhs));
            } else break;
        } while(true);
        return lhs;
    }

    ptr_value parsePrec10ValueExpression() {
        // Precedence 10 operators: Cumulative Operators
        // <Prec10Exp> ::= <Prec11Exp> | <Add> | <Subtract>

        // <Prec11Exp>
        ptr_value prec10Exp = parsePrec11ValueExpression();
        do {
            if (is<TokenType::Plus>()) {
                // <Add> ::= <Prec10Exp> plus <Prec11Exp>
                consume<TokenType::Plus>();
                ptr_value rhs = parsePrec11ValueExpression();
                prec10Exp = std::make_unique<AddExpression>(move(prec10Exp), move(rhs));
            } else if (is<TokenType::Minus>()) {
                // <Subtract> ::= <Prec10Exp> minus <Prec11Exp>
                consume<TokenType::Minus>();
                ptr_value rhs = parsePrec11ValueExpression();
                prec10Exp = std::make_unique<SubtractExpression>(move(prec10Exp), move(rhs));
            } else break;
        } while(true);
        return prec10Exp;
    }

    ptr_value parsePrec11ValueExpression() {
        // Precedence 11 operators: Multiplicative Operators
        // <Prec11Exp> ::= <Prec12Exp> | <Multiply> | <Divide> | <Reminder>

        // <Prec12Exp>
        ptr_value prec11Exp = parsePrec12ValueExpression();
        do {
            if (is<TokenType::Multiply>()) {
                // <Multiply> ::= <Prec11Exp> multiply <Prec12Exp>
                consume<TokenType::Multiply>();
                ptr_value rhs = parsePrec12ValueExpression();
                prec11Exp = std::make_unique<MultiplyExpression>(move(prec11Exp), move(rhs));
            } else if (is<TokenType::Divide>()) {
                // <Divide> ::= <Prec11Exp> divide <Prec12Exp>
                consume<TokenType::Divide>();
                ptr_value rhs = parsePrec12ValueExpression();
                prec11Exp = std::make_unique<DivideExpression>(move(prec11Exp), move(rhs));
            } else if (is<TokenType::Reminder>()) {
                // <Reminder> ::= <Prec11Exp> reminder <Prec12Exp>
                consume<TokenType::Reminder>();
                ptr_value rhs = parsePrec12ValueExpression();
                prec11Exp = std::make_unique<ReminderExpression>(move(prec11Exp), move(rhs));
            } else break;
        } while(true);
        return prec11Exp;
    }

    ptr_value parsePrec12ValueExpression() {
        // Precedence 12 operators: Unary Pre-Operators
        // <Prec12Exp> ::= 
        //             | <Not>
        //             | <Plus>
        //             | <Minus>
        //             | <PreIncrement>
        //             | <PreDecrement>
        //             | <BitComplement>
        //             | <Prec13Exp>

        if (is<TokenType::ConditionalNot>()) {
            consume<TokenType::ConditionalNot>();
            return std::make_unique<ConditionalNotExpression>(parsePrec12ValueExpression());
        } else if (is<TokenType::Plus>()) {
            consume<TokenType::Plus>();
            return std::make_unique<UnaryPlusExpression>(parsePrec12ValueExpression());
        } else if (is<TokenType::Minus>()) {
            consume<TokenType::Minus>();
            return std::make_unique<UnaryMinusExpression>(parsePrec12ValueExpression());
        } else if (is<TokenType::Increment>()) {
            consume<TokenType::Increment>();
            return std::make_unique<PreIncrementExpression>(parsePrec12ValueExpression());
        } else if (is<TokenType::Decrement>()) {
            consume<TokenType::Decrement>();
            return std::make_unique<PreDecrementExpression>(parsePrec12ValueExpression());
        } else if (is<TokenType::BitComplement>()) {
            consume<TokenType::BitComplement>();
            return std::make_unique<BitComplementExpression>(parsePrec12ValueExpression());
        } else return parsePrec13ValueExpression();
    }

    ptr_value parsePrec13ValueExpression() {
        ptr_value prec13Exp = nullptr;
        // Precedence 13 operators: Unary Post-Operators and Base Expressions
        // <Prec13Exp> ::=
        // <Identifier>
        if (is<TokenType::Identifier>()) prec13Exp = std::make_unique<IdentifierExpression>(consume<TokenType::Identifier>());
        // <ConstantValue>
        else if (is<TokenType::Float>()) prec13Exp = std::make_unique<ConstantValueExpression>(consume<TokenType::Float>());
        else if (is<TokenType::Integer>()) prec13Exp = std::make_unique<ConstantValueExpression>(consume<TokenType::Integer>());
        else if (is<TokenType::SingleQuotesString>()) prec13Exp = std::make_unique<ConstantValueExpression>(consume<TokenType::SingleQuotesString>());
        else if (is<TokenType::DoubleQuotesString>()) prec13Exp = std::make_unique<ConstantValueExpression>(consume<TokenType::DoubleQuotesString>());
        // <ParenExpression>
        else if (is<TokenType::OpenParenthesis>()) {
            consume<TokenType::OpenParenthesis>();
            prec13Exp = parseValueExpression();
            consume<TokenType::CloseParenthesis>();
        }
        else throw CompilerError(UnexpectedParserEoF, span());

        do {
            // <MemberAccess> ::= <Prec13Exp> dot <Member>
            if (is<TokenType::Dot>()) {
                consume<TokenType::Dot>();
                // <Member> ::= Identifier
                auto name = consume<TokenType::Identifier>();
                prec13Exp = std::make_unique<MemberAccessExpression>(move(prec13Exp), name);
            } else if (is<TokenType::OpenParenthesis>()) {
                // <Invocation> ::= <Prec13Exp> l-paren <Values> r-paren
                consume<TokenType::OpenParenthesis>();
                // <Values> ::= <Expression> comma <Values> | <Expression> | <>
                vector<ptr_value> values;
                if (!is<TokenType::CloseParenthesis>()) {
                    do {
                        values.push_back(parseValueExpression());
                        if (is<TokenType::Comma>()) consume<TokenType::Comma>();
                        else break;
                    } while(true);
                }
                consume<TokenType::CloseParenthesis>();
                prec13Exp = std::make_unique<InvocationExpression>(move(prec13Exp), move(values));
            } else if (is<TokenType::Increment>()) {
                // <PostIncrement> ::= <Prec13Exp> increment
                consume<TokenType::Increment>();
                prec13Exp = std::make_unique<PostIncrementExpression>(move(prec13Exp));
            } else if (is<TokenType::Decrement>()) {
                // <PostDecrement> ::= <Prec13Exp> decrement
                consume<TokenType::Decrement>();
                prec13Exp = std::make_unique<PostDecrementExpression>(move(prec13Exp));
            } else break;
        } while(true);
        return prec13Exp;
    }

    unique_ptr<ConstStatement> parseConstStatement() {
        consume<TokenType::ConstKeyword>();
        auto name = consume<TokenType::Identifier>();

        ptr_type type = nullptr;
        if (is<TokenType::Colon>()) {
            consume<TokenType::Colon>();
            type = parseTypeExpression();
        }

        consume<TokenType::Assignment>();
        auto value = parseValueExpression();
        consume<TokenType::Semicolon>();
        return std::make_unique<ConstStatement>(name, move(type), move(value));
    }

    unique_ptr<ReturnStatement> parseReturnStatement() {
        consume<TokenType::ReturnKeyword>();
        ptr_value value = nullptr;
        if (!is<TokenType::Semicolon>()) value = parseValueExpression();
        consume<TokenType::Semicolon>();
        return std::make_unique<ReturnStatement>(move(value));
    }

    ptr_statement parseStatement() {
        if (is<TokenType::ConstKeyword>()) return parseConstStatement();
        if (is<TokenType::ReturnKeyword>()) return parseReturnStatement();

        // TODO: for, if, etc.

        ptr_statement expression = parseValueExpression();
        consume<TokenType::Semicolon>();
        return expression;
    }

    unique_ptr<CodeBlock> parseCodeBlock() {
        consume<TokenType::LeftBrace>();
        vector<ptr_statement> _statements;
        while(!is<TokenType::RightBrace>()) {
            if (is<TokenType::EoF>()) throw CompilerError(UnexpectedParserEoF, span());
            _statements.push_back(parseStatement());
        }
        consume<TokenType::RightBrace>();
        return std::make_unique<CodeBlock>(move(_statements));
    }

    ptr_type parseTypeExpression() {
        if (is<TokenType::IntKeyword>()) {
            return std::make_unique<PrimitiveTypeExpression>(consume<TokenType::IntKeyword>(), PrimitiveType::Int);
        } else if (is<TokenType::FloatKeyword>()) {
            return std::make_unique<PrimitiveTypeExpression>(consume<TokenType::FloatKeyword>(), PrimitiveType::Float);
        } else if (is<TokenType::StringKeyword>()) {
            return std::make_unique<PrimitiveTypeExpression>(consume<TokenType::StringKeyword>(), PrimitiveType::String);
        } else if (is<TokenType::BoolKeyword>()) {
            return std::make_unique<PrimitiveTypeExpression>(consume<TokenType::BoolKeyword>(), PrimitiveType::Bool);
        }

        // TODO: Identifier, fully qualified names, generics, etc.
        throw CompilerError(UnexpectedToken, span());
    }

    unique_ptr<FunctionArgument> consumeFunctionArgument() {
        Token identifier;
        if (is<TokenType::Identifier>()) {
            identifier = consume<TokenType::Identifier>();
        }
        consume<TokenType::Colon>();
        ptr_type type = parseTypeExpression();

        return std::make_unique<FunctionArgument>(identifier, move(type));
    }

    unique_ptr<FunctionPrototype> consumeFunctionPrototype() {
        vector<unique_ptr<FunctionArgument> > args;

        consume<TokenType::OpenParenthesis>();
        if (!is<TokenType::CloseParenthesis>()) {
            args.push_back(consumeFunctionArgument());
            while(is<TokenType::Comma>()) {
                consume<TokenType::Comma>();
                args.push_back(consumeFunctionArgument());
            }
        }
        consume<TokenType::CloseParenthesis>();

        ptr_type returnTypeAnnotation = nullptr;
        if (is<TokenType::Colon>()) {
            consume<TokenType::Colon>();
            returnTypeAnnotation = parseTypeExpression();
        }

        return std::make_unique<FunctionPrototype>(move(args), move(returnTypeAnnotation));
    }

    unique_ptr<Function> consumeFunction() {
        auto functionKeywordToken = consume<TokenType::FunctionKeyword>();
        auto identifier = consume<TokenType::Identifier>();
        auto proto = consumeFunctionPrototype();
        auto block = parseCodeBlock();

        // TODO: Add args, add return type, add code block.
        return std::make_unique<Function>(identifier, move(proto), move(block));
    }

    unique_ptr<FunctionDeclaration> consumeFunctionDeclaration(Token& declareKeyword) {
        auto functionKeywordToken = consume<TokenType::FunctionKeyword>();
        auto identifier = consume<TokenType::Identifier>();
        auto proto = consumeFunctionPrototype();
        auto semicolon = consume<TokenType::Semicolon>();

        return std::make_unique<FunctionDeclaration>(identifier, move(proto));
    }

    void parseModuleMembers() {
        while(!is<TokenType::EoF>()) {
            while(is<TokenType::Whitespace>()) consume<TokenType::Whitespace>();
            if (is<TokenType::FunctionKeyword>()) {
                _members.push_back(consumeFunction());
            } else if (is<TokenType::DeclareKeyword>()) {
                Token declareKeyword = consume<TokenType::DeclareKeyword>();
                if (is<TokenType::FunctionKeyword>()) {
                    _members.push_back(consumeFunctionDeclaration(declareKeyword));
                } else {
                    throw CompilerError(UnexpectedToken, span());
                }
            }
            else throw CompilerError(UnexpectedToken, span());
        }
    }
};

const string Parser::UnaryOperatorString[8] = {
    // Precedence 12
    "!"s, // ConditionalNotOperator
    "+"s, // UnaryPlusOperator
    "-"s, // UnaryMinusOperator
    "++"s, // PreIncrementOperator
    "--"s, // PreDecrementOperator
    "~"s, // BitComplementOperator
    // Precedence 13
    "++"s, // PostIncrementOperator,
    "--"s, // PostDecrementOperator,
};

const string Parser::BinaryOperatorString[29] = {
    // Precedence 0
    "="s, // AssignmentOperator,
    "+="s, // AdditionAssignmentOperator,
    "-="s, // SubtractionAssignmentOperator,
    "*="s, // MultiplicationAssignmentOperator,
    "/="s, // DivisionAssignmentOperator,
    "&="s, // BitAndAssignmentOperator,
    "^="s, // BitXOrAssignmentOperator,
    "|="s, // BitOrAssignmentOperator,
    "%="s, // ReminderAssignmentOperator,
    "<<="s, // LeftShiftAssignmentOperator,
    ">>="s, // RightShiftAssignmentOperator,
    // Precedence 2
    "||"s, // ConditionalOrOperator,
    // Precedence 3
    "&&"s, // ConditionalAndOperator,
    // Precedence 4
    "|"s, // BitwiseOrOperator,
    // Precedence 5
    "^"s, // BitwiseXOrOperator,
    // Precedence 6
    "&"s, // BitwiseAndOperator,
    // Precedence 7
    "=="s, // EqualOperator,
    "!="s, // NotEqualOperator,
    // Precedence 8
    "<"s, // LessThanOperator,
    ">"s, // GreaterThanOperator,
    "<="s, // LessOrEqualOperator,
    ">="s, // GreaterOrEqualOperator,
    // Precedence 9
    "<<"s, // LeftShiftOperator,
    ">>"s, // RightShiftOperator,
    // Precedence 10
    "+"s, // AddOperator,
    "-"s, // SubtractOperator,
    // Precedence 11
    "*"s, // MultiplyOperator,
    "/"s, // DivideOperator,
    "%"s, // ReminderOperator,
};

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

// Debug AST printer
class ASTPrinter : public Parser::ModuleMemberVisitor, public Parser::TypeExpressionVisitor, public Parser::StatementVisitor {
public:
    void on(Parser::Function* f) override {
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

    void on(Parser::FunctionDeclaration* f) override {
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

    void on(Parser::PrimitiveTypeExpression* t) override {
        switch(t->type()) {
            case Parser::PrimitiveType::Int:
                cout << "int";
                break;
            case Parser::PrimitiveType::Float:
                cout << "float";
                break;
            case Parser::PrimitiveType::String:
                cout << "string";
                break;
            case Parser::PrimitiveType::Bool:
                cout << "bool";
                break;
        }
    }

    // Statements
    void on(Parser::CodeBlock* statement) override {
        cout << " {" << endl;
        for (auto& statement : statement->statements()) {
            statement->visit(this);
        }
        cout << "}" << endl;
    }
    void on(Parser::ConstStatement* statement) override {
        cout << "const " << statement->name();
        Parser::ptr_type& type = statement->type();
        if (type != nullptr) {
            cout << ": ";
            type->visit(this);
        }
        Parser::ptr_value& value = statement->value();
        cout << " = ";
        value->visit(this);
        cout << ";" << endl;
    }
    void on(Parser::ReturnStatement* statement) override {
        cout << "return";
        Parser::ptr_value& value = statement->value();
        if (value != nullptr) {
            cout << " ";
            value->visit(this);
        }
        cout << ";" << endl;
    }

    // Value expressions
    void on(Parser::IdentifierExpression* expression) override {
        cout << expression->name();
    }
    void on(Parser::ConstantValueExpression* expression) override {
        cout << expression->literal().code();
    }
    void on(Parser::MemberAccessExpression* expression) override {
        cout << "?";
    }
    void on(Parser::InvocationExpression* expression) override {
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
    void on(Parser::ConditionalIfExpression* expression) override {
        cout << "?";
    }

    void on(Parser::UnaryExpression* expression) override {
        switch(expression->op()) {
            case Parser::UnaryOperator::ConditionalNotOperator: cout << "!"s; break;
            case Parser::UnaryOperator::UnaryPlusOperator: cout << "+"s; break;
            case Parser::UnaryOperator::UnaryMinusOperator: cout << "-"s; break;
            case Parser::UnaryOperator::PreIncrementOperator: cout << "++"s; break;
            case Parser::UnaryOperator::PreDecrementOperator: cout << "--"s; break;
            case Parser::UnaryOperator::BitComplementOperator: cout << "~"s; break;
            default:;
        }

        expression->operand()->visit(this);

        switch(expression->op()) {
            case Parser::UnaryOperator::PostIncrementOperator: cout << "++"s; break;
            case Parser::UnaryOperator::PostDecrementOperator: cout << "--"s; break;
            default:;
        }
    }
    void on(Parser::BinaryExpression* expression) override {
        expression->lhs()->visit(this);
        cout << " "s << Parser::toString(expression->op()) << " "s;
        expression->rhs()->visit(this);
    }
};

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

class LLIRCompiler : public Parser::ModuleMemberVisitor {
public:
    void on(Parser::Function* f) override {
        cout << "compile " << f->name() << endl;

        // Make the function type:  double(double,double) etc.
        std::vector<Type*> Doubles(f->args().size(), Type::getFloatTy(TheContext));
        FunctionType *FT = FunctionType::get(Type::getFloatTy(TheContext), Doubles, false);
        Function *F = Function::Create(FT, Function::ExternalLinkage, f->name(), TheModule.get());

        unsigned i = 0;
        for (auto &Arg : F->args()) {
            auto name = f->args()[i++]->name();
            Arg.setName(name);
        }
    }

    void on(Parser::FunctionDeclaration* f) override {
        cout << "compile " << f->name() << endl;

        // Make the function type:  double(double,double) etc.
        std::vector<Type*> Doubles(f->args().size(), Type::getFloatTy(TheContext));
        FunctionType *FT = FunctionType::get(Type::getFloatTy(TheContext), Doubles, false);
        Function *F = Function::Create(FT, Function::ExternalLinkage, f->name(), TheModule.get());

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

    // TODO: Typechecking!

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
