#include <iostream>
#include <vector>
#include <map>
#include <cassert>
#include <exception>
#include <variant>

using namespace std;

/// LEXER ///

enum TokenType {
    EoF = 0,

    // Complex expressions
    Identifier, // a-zA-Z_ and any code >127 followed by a-zA-Z_0-9 and any code >127
    Integer, // uint8, uint16, uint32, uint64
    Float, // float32, float64
    Whitespace, // \r\n, white space, tabulation
    DoubleQuotesString, // ""
    SingleQuotesString, // ''

    // Keywords
    DeclareKeyword, // declare
    FunctionKeyword, // function
    VoidKeyword, // void
    NeverKeyword, // never
    BoolKeyword, // bool
    IntKeyword, // int
    FloatKeyword, // float
    StringKeyword, // string
    VarKeyword, // var
    ConstKeyword, // const
    ReturnKeyword, // return

    // Operator symbols
    OpenParenthesis, // (
    CloseParenthesis, // )
    LeftBracket, // [
    RightBracket, // ]
    LeftBrace, // {
    RightBrace, // }
    Colon, // :
    Semicolon, // ;
    Comma, // ,
    Dot, // .

    Assignment, // =
    Equal, // ==
    Less, // <
    LessOrEqual, // <=
    Greater, // >
    GreaterOrEqual, // >=
    NotEqual, // !=

    Plus, // +
    Minus, // -
    Multiply, // *
    Divide, // /
    Reminder, // % calculates the reminder after dividing

    BitwiseAnd, // &
    BitwiseOr, // |
    BitwiseXOr, // ^

    Question, // ?
    BitComplement, // ~
    
    Increment, // ++
    Decrement, // --

    ConditionalAnd, // &&
    ConditionalOr, // ||
    ConditionalNot, // !
    
    LeftShift, // <<
    RightShift, // >>

    AdditionAssignment, // +=
    SubtractionAssignment, // -=
    MultiplicationAssignment, // *=
    DivisionAssignment, // /=
    ReminderAssignment, // %=

    BitAndAssignment, // &=
    BitOrAssignment, // |=
    BitXOrAssignment, // ^=
    
    LeftShiftAssignment, // <<=
    RightShiftAssignment, // >>=

    PointerMemberAccess, // ->
    NullCoalescing, // ??
    LambdaOperator, // =>
};

string tokenTypeNames[] = {
    "EoF",

    // Complex expressions
    "Identifier", // a-zA-Z_ and any code >127 followed by a-zA-Z_0-9 and any code >127
    "Integer", // uint8, uint16, uint32, uint64
    "Float", // float32, float64
    "Whitespace", // \r\n, white space, tabulation
    "DoubleQuotesString", // ""
    "SingleQuotesString", // ''

    // Keywords
    "DeclareKeyword", // declare
    "FunctionKeyword", // function
    "VoidKeyword", // void
    "NeverKeyword", // never
    "BoolKeyword", // bool
    "IntKeyword", // int
    "FloatKeyword", // float
    "StringKeyword",
    "VarKeyword", // var
    "ConstKeyword", // const
    "ReturnKeyword", // return

    // Operator symbols
    "OpenParenthesis", // (
    "CloseParenthesis", // )
    "LeftBracket", // [
    "RightBracket", // ]
    "LeftBrace", // {
    "RightBrace", // }
    "Colon", // :
    "Semicolon", // ;
    "Comma", // ,
    "Dot", // .

    "Assignment", // =
    "Equal", // ==
    "Less", // <
    "LessOrEqual", // <=
    "Greater", // >
    "GreaterOrEqual", // >=
    "NotEqual", // !=

    "Plus", // +
    "Minus", // -
    "Multiply", // *
    "Divide", // /
    "Reminder", // % calculates the reminder after dividing

    "BitwiseAnd", // &
    "BitwiseOr", // |
    "BitwiseXOr", // ^

    "Question", // ?
    "BitComplement", // ~
    
    "Increment", // ++
    "Decrement", // --

    "ConditionalAnd", // &&
    "ConditionalOr", // ||
    "ConditionalNot", // !
    
    "LeftShift", // <<
    "RightShift", // >>

    "AdditionAssignment", // +=
    "SubtractionAssignment", // -=
    "MultiplicationAssignment", // *=
    "DivisionAssignment", // /=
    "ReminderAssignment", // %=

    "BitAndAssignment", // &=
    "BitOrAssignment", // |=
    "BitXOrAssignment", // ^=
    
    "LeftShiftAssignment", // <<=
    "RightShiftAssignment", // >>=

    "PointerMemberAccess", // ->
    "NullCoalescing", // ??
    "LambdaOperator", // =>
};

string name(TokenType type) {
    return tokenTypeNames[type];
}

map<string, TokenType> keywords {
    { "declare", DeclareKeyword },
    { "function", FunctionKeyword },
    { "void"s, VoidKeyword },
    { "never"s, NeverKeyword },
    { "bool"s, BoolKeyword },
    { "int"s, IntKeyword },
    { "float"s, FloatKeyword },
    { "string"s, StringKeyword },
    { "var"s, VarKeyword },
    { "const"s, ConstKeyword },
    { "return"s, ReturnKeyword },
};

map<string, TokenType> operatorSymbols {
    { "("s, OpenParenthesis },
    { ")"s, CloseParenthesis },
    { "["s, LeftBracket },
    { "]"s, RightBracket },
    { "{"s, LeftBrace },
    { "}"s, RightBrace },
    { ":"s, Colon },
    { ";"s, Semicolon },
    { ","s, Comma },
    { "."s, Dot },

    { "="s, Assignment },
    { "=="s, Equal },
    { "<"s, Less },
    { "<="s, LessOrEqual },
    { ">"s, Greater },
    { ">="s, GreaterOrEqual },
    { "!="s, NotEqual },

    { "+"s, Plus },
    { "-"s, Minus },
    { "*"s, Multiply },
    { "/"s, Divide },
    { "%"s, Reminder },

    { "&"s, BitwiseAnd },
    { "|"s, BitwiseOr },
    { "^"s, BitwiseXOr },

    { "?"s, Question },
    { "~"s, BitComplement },
    
    { "++"s, Increment },
    { "--"s, Decrement },

    { "&&"s, ConditionalAnd },
    { "||"s, ConditionalOr },
    { "!"s, ConditionalNot },
    
    { "<<"s, LeftShift },
    { ">>"s, RightShift },

    { "+="s, AdditionAssignment },
    { "-="s, SubtractionAssignment },
    { "*="s, MultiplicationAssignment },
    { "/="s, DivisionAssignment },
    { "%="s, ReminderAssignment },

    { "&="s, BitAndAssignment },
    { "|="s, BitOrAssignment },
    { "^="s, BitXOrAssignment },
    
    { "<<="s, LeftShiftAssignment },
    { ">>="s, RightShiftAssignment },

    { "->", PointerMemberAccess },
    { "??", NullCoalescing },
    { "=>", LambdaOperator },
};

enum ErrorCode {
    // Lexer errors
    UnexpectedEoF = 1001,
    UnexpectedCharacter = 1002,
    ExpectedADigit = 1003,
    NumberOverflow = 1004,

    // Parser errors
    UnexpectedParserEoF = 2001,
    UnexpectedToken = 2002,
    ExpectedIdentifier = 2003,
};

map<ErrorCode, string> errorCodeDescription = {
    // Lexer errors
    { UnexpectedEoF, "Lexer error, unexpected end of file."s },
    { UnexpectedCharacter, "Lexed error, unexpected character."s },
    { ExpectedADigit, "Lexed error, expected a digit from 0 to 9."s },
    { NumberOverflow, "Lexed error, number too large."s },

    // Parser errors
    { UnexpectedEoF, "Parser error, unexpected end of file."s },
    { UnexpectedToken, "Parser error, unexpected token."s },
    { ExpectedIdentifier, "Parser error, expected an identifier."s },
};

typedef typename string::iterator StringIterator;
typedef typename string::value_type Char;

struct SourcePoint {
    unsigned int line, column;
    SourcePoint() : line(1), column(1) {}

    string toString() const {
        return to_string(line) + ":"s + to_string(column);
    }

    bool operator==(const SourcePoint& other) const {
        return line == other.line && column == other.column;
    }
};

struct SourceSpan {
    SourcePoint start;
    SourcePoint end;

    string toString() const {
        return start == end ? start.toString() : start.toString() + "-"s + end.toString();
    }

    bool operator==(const SourceSpan& other) const {
        return start == other.start && end == other.end;
    }
};

struct StringSpan {
    StringIterator start;
    StringIterator end;

    string toString() const { return string(start, end); }
};

ostream& operator<< (ostream& out, const SourcePoint& point) {
    return out << point.line << ":" << point.column;
}

ostream& operator<< (ostream& out, const SourceSpan& span) {
    if (span.start == span.end) {
        return out << span.start;
    } else {
        return out << span.start << "-" << span.end;
    }
}

class CompilerError : public exception {
    ErrorCode _code;
    SourceSpan _span;

    string _message;

public:
    CompilerError(ErrorCode code, SourceSpan span) :
        _code(code),
        _span(span),
        _message(errorCodeDescription[code] + " ("s + span.toString() + ")") {
    }

    CompilerError(ErrorCode code, SourcePoint point) : CompilerError(code, {point, point}) {}

    SourceSpan span() { return _span; }

    virtual const char* what() const throw() { return _message.c_str(); }
};

class Lexer {
    string _code;

public:
    Lexer(string code): _code(code) {}

    class TokenIterator;
    class Token;

    class Token {
        TokenType _type;
        variant<unsigned long long, double, string> _value;

        SourceSpan _sourceSpan;
        StringSpan _stringSpan;

    public:
        friend class TokenIterator;

        Token() : _type(TokenType::EoF), _value(0ULL) {}

        const TokenType& type() const { return _type; }

        template<typename T>
        T value() { return get<T>(_value); }

        const SourceSpan span() const { return _sourceSpan; }
        string code() const { return _stringSpan.toString(); }
        
        bool operator == (const TokenType type) const { return _type == type; }
        bool operator != (const TokenType type) const { return _type != type; }
        operator bool() const { return _type != EoF; }
        operator TokenType() const { return _type; }
    };

    class TokenIterator {
        StringIterator _current;
        StringIterator _end;

        Token _token;
        SourcePoint _point;
        Char _lookaheadChar;

    public:
        typedef input_iterator_tag iterator_category;

        TokenIterator(StringIterator begin, StringIterator end) :
            _current(begin),
            _end(end),
            _lookaheadChar(_current == _end ? 0 : *_current) {

            consumeNext();
        }

        TokenIterator& operator++() {
            consumeNext();
            return *this;
        }

        TokenIterator operator++(int) {
            TokenIterator result(*this);
            ++(*this);
            return result;
        }

        bool operator == (const TokenIterator& other) {
            return _end == other._end
                && _current == other._current
                && _token._type == other._token._type;
        }

        bool operator != (const TokenIterator& other) {
            return _end != other._end
                || _current != other._current
                || _token._type != other._token._type;
        }

        const Token& operator*() const { return _token; }
        const Token* operator->() const { return &_token; }

    private:

        Char consumeChar() {
            Char c = _lookaheadChar;
            if (c == '\n') {
                _point.column = 1;
                _point.line++;
            } else {
                _point.column++;
            }
            _lookaheadChar = _current == _end ? 0 : *++_current;
            return c;
        }

        bool isEoF() { return _lookaheadChar == 0; }
        bool isChar() { return (_lookaheadChar >= 'a' && _lookaheadChar <= 'z') || (_lookaheadChar >= 'A' && _lookaheadChar <= 'Z') || (_lookaheadChar > 127); }
        bool isNumber() { return _lookaheadChar >= '0' && _lookaheadChar <= '9'; }        
        bool isWhitespace() { return _lookaheadChar == ' ' || _lookaheadChar == '\t' || _lookaheadChar == '\n' || _lookaheadChar == '\r'; }
        bool isIdentifierLeadChar() { return isChar() || _lookaheadChar == '_'; }
        bool isOperatorLeadChar() {
            string op;
            op += _lookaheadChar;
            return operatorSymbols.find(op) != operatorSymbols.end();
        }
        bool isIdentifierTailChar() { return isIdentifierLeadChar() || isNumber(); }
        bool isDoubleQuoteChar() { return _lookaheadChar == '"'; }
        bool isSingleQuoteChar() { return _lookaheadChar == '\''; }

        void consumeIdentifier() {
            string identifier;
            assert(isIdentifierLeadChar());
            identifier += consumeChar();

            while(isIdentifierTailChar()) identifier += consumeChar();

            auto keyword = keywords.find(identifier);
            if (keyword == keywords.end()) {
                _token._type = TokenType::Identifier;
                _token._value = identifier;
            } else {
                _token._type = keyword->second;
                _token._value = 0ULL;
            }
        }

        void consumeSingleCharToken(const Char& c, const TokenType& type) {
            ++_point.column;
            _token._type = type;
            _token._value = 0ULL;
        }

        void consumeWhitespace() {
            while(isWhitespace()) consumeChar();
            _token._type = TokenType::Whitespace;
            _token._value = 0ULL;
        }

        void consumeString() {
            Char terminator = consumeChar();
            assert(terminator == '\'' || terminator == '"');
            string value;
            do {
                if (isEoF()) throw CompilerError(UnexpectedEoF, _point);
                if (_lookaheadChar == '\r' || _lookaheadChar == '\n') throw CompilerError(UnexpectedCharacter, _point);

                if (_lookaheadChar == '\\') {
                    assert(false); // TODO: Handle escape sequences...
                } else if (_lookaheadChar == terminator) {
                    consumeChar();
                    _token._value = value;
                    return;
                } else {
                    value += consumeChar();
                }
            }
            while(true);
        }

        void consumeDoubleQuotesString() {
            assert(_lookaheadChar == '"');
            _token._type = TokenType::DoubleQuotesString;
            consumeString();
        }

        void consumeSingleQuotesString() {
            assert(_lookaheadChar == '\'');
            _token._type = TokenType::SingleQuotesString;
            consumeString();
        }

        void consumeNumber() {
            assert(isNumber());
            string number;
            Char lead = consumeChar();
            number += lead;

            bool hasFrac = false;
            bool hasExp = false;

            if (lead == '0' && isNumber()) throw CompilerError(UnexpectedCharacter, _point);

            while(isNumber()) number += consumeChar();

            if (_lookaheadChar == '.') {
                number += consumeChar();
                hasFrac = true;

                if (isEoF()) throw CompilerError(UnexpectedEoF, _point);
                if (!isNumber()) throw CompilerError(ExpectedADigit, _point);

                while(isNumber()) number += consumeChar();
            }

            if (_lookaheadChar == 'e' || _lookaheadChar == 'E') {
                number += consumeChar();
                hasExp = true;
                if (isEoF()) throw CompilerError(UnexpectedEoF, _point);
                if (_lookaheadChar == '+' || _lookaheadChar == '-') number += consumeChar();
                if (!isNumber()) throw CompilerError(ExpectedADigit, _point);
                while(isNumber()) number += consumeChar();
            }

            try {
                if (hasExp || hasFrac) {   
                    _token._value = stod(number);
                    _token._type = Float;
                } else {
                    _token._value = stoull(number);
                    _token._type = Integer;
                }
            } catch(out_of_range e) {
                throw CompilerError(NumberOverflow, _point);
            } catch(invalid_argument e) {
                assert(false); // Numbers should be parsable by C++ std::stod() or std::stoull()
            }
        }

        void consumeOperator() {
            string op;
            op += _lookaheadChar;
            auto found = operatorSymbols.find(op);
            if (found == operatorSymbols.end()) {
                assert(false); // Should have checked isOperatorChar() first.
            }
            consumeChar();
            do {
                op += _lookaheadChar;
                auto longerOpFound = operatorSymbols.find(op);
                if (longerOpFound == operatorSymbols.end()) {
                    _token._type = found->second;
                    _token._value = 0ULL;
                    return;
                }
                found = longerOpFound;
                consumeChar();
            } while(true);
        }

        void consumeNext() {
            _token._sourceSpan.start = _point;
            _token._stringSpan.start = _current;

            if (isEoF()) {
                _token._type = TokenType::EoF;
                _token._value = 0ULL;
            } else if (isWhitespace()) {
                consumeWhitespace();
            } else if (isIdentifierLeadChar()) {
                consumeIdentifier();
            } else if (isDoubleQuoteChar()) {
                consumeDoubleQuotesString();
            } else if (isSingleQuoteChar()) {
                consumeSingleQuotesString();
            } else if (isNumber()) {
                consumeNumber();
            } else if (isOperatorLeadChar()) {
                consumeOperator();
            } else {
                throw CompilerError(UnexpectedCharacter, _point);
            }

            _token._sourceSpan.end = _point;
            _token._stringSpan.end = _current;
        }
    };

    TokenIterator begin() {
        return TokenIterator(_code.begin(), _code.end());
    }
    
    TokenIterator end() {
        StringIterator end = _code.end();
        return TokenIterator(end, end);
    }
};

ostream& operator << (ostream& out, Lexer::Token& token) {
    out << name(token.type()) << " (" << token.span() << ") ";
    switch(token.type()) {
        case Identifier:
            out << token.value<string>() << " ";
            break;
        case SingleQuotesString:
            out << '\'' << token.value<string>() << '\'' << " ";
            break;
        case DoubleQuotesString:
            out << '"' << token.value<string>() << '"' << " ";
            break;
        case Integer:
            out << token.value<unsigned long long int>() << " ";
            break;
        case Float:
            out << token.value<double>() << " ";
            break;
        default:
            break;
    }

    return out;
}

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
    protected:
        friend class Parser;
        virtual void on(Function* f) = 0;
        virtual void on(FunctionDeclaration* f) = 0;
    };

    class TypeExpressionVisitor {
    protected:
        friend class PrimitiveTypeExpression;
        virtual void on(PrimitiveTypeExpression* t) = 0;
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
    };

    class CodeBlock : public Statement {
        vector<ptr_statement> _statements;
    public:
        CodeBlock(vector<ptr_statement> statements) : _statements(move(statements)) {}
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
    };

    class ReturnStatement : public Statement {
        ptr_value _value;
    public:
        ReturnStatement(ptr_value value) : _value(move(value)) {}
    };

    class ValueExpression : public Statement {
    public:
        virtual ushort precedence() = 0;
    };

    class UnaryExpression : public ValueExpression {
        ptr_value _operand;
    public:
        UnaryExpression(ptr_value operand) : _operand(move(operand)) {}
        ptr_value& operand() { return _operand; }
    };

    class BinaryExpression : public ValueExpression {
        ptr_value _lhs, _rhs;
    public:
        BinaryExpression(ptr_value lhs, ptr_value rhs) : _lhs(move(lhs)), _rhs(move(rhs)) {}
        ptr_value& lhs() { return _lhs; }
        ptr_value& rhs() { return _rhs; }
    };

    // Precedence 0
    // TODO: Use some wild macro to define all these classes and reduce code-size.
    class AssignmentExpression : public BinaryExpression {
    public:
        AssignmentExpression(ptr_value lhs, ptr_value rhs) : BinaryExpression(move(lhs), move(rhs)) {}
        ushort precedence() override { return 0; }
    };
    class AdditionAssignmentExpression : public BinaryExpression {
    public:
        AdditionAssignmentExpression(ptr_value lhs, ptr_value rhs) : BinaryExpression(move(lhs), move(rhs)) {}
        ushort precedence() override { return 0; }
    };
    class SubtractionAssignmentExpression : public BinaryExpression {
    public:
        SubtractionAssignmentExpression(ptr_value lhs, ptr_value rhs) : BinaryExpression(move(lhs), move(rhs)) {}
        ushort precedence() override { return 0; }
    };
    class MultiplicationAssignmentExpression : public BinaryExpression {
    public:
        MultiplicationAssignmentExpression(ptr_value lhs, ptr_value rhs) : BinaryExpression(move(lhs), move(rhs)) {}
        ushort precedence() override { return 0; }
    };
    class DivisionAssignmentExpression : public BinaryExpression {
    public:
        DivisionAssignmentExpression(ptr_value lhs, ptr_value rhs) : BinaryExpression(move(lhs), move(rhs)) {}
        ushort precedence() override { return 0; }
    };
    class BitAndAssignmentExpression : public BinaryExpression {
    public:
        BitAndAssignmentExpression(ptr_value lhs, ptr_value rhs) : BinaryExpression(move(lhs), move(rhs)) {}
        ushort precedence() override { return 0; }
    };
    class BitXOrAssignmentExpression : public BinaryExpression {
    public:
        BitXOrAssignmentExpression(ptr_value lhs, ptr_value rhs) : BinaryExpression(move(lhs), move(rhs)) {}
        ushort precedence() override { return 0; }
    };
    class BitOrAssignmentExpression : public BinaryExpression {
    public:
        BitOrAssignmentExpression(ptr_value lhs, ptr_value rhs) : BinaryExpression(move(lhs), move(rhs)) {}
        ushort precedence() override { return 0; }
    };
    class ReminderAssignmentExpression : public BinaryExpression {
    public:
        ReminderAssignmentExpression(ptr_value lhs, ptr_value rhs) : BinaryExpression(move(lhs), move(rhs)) {}
        ushort precedence() override { return 0; }
    };
    class LeftShiftAssignmentExpression : public BinaryExpression {
    public:
        LeftShiftAssignmentExpression(ptr_value lhs, ptr_value rhs) : BinaryExpression(move(lhs), move(rhs)) {}
        ushort precedence() override { return 0; }
    };
    class RightShiftAssignmentExpression : public BinaryExpression {
    public:
        RightShiftAssignmentExpression(ptr_value lhs, ptr_value rhs) : BinaryExpression(move(lhs), move(rhs)) {}
        ushort precedence() override { return 0; }
    };

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
    };

    // Precedence 2
    class ConditionalOrExpression : public BinaryExpression {
    public:
        ConditionalOrExpression(ptr_value lhs, ptr_value rhs) : BinaryExpression(move(lhs), move(rhs)) {}
        ushort precedence() override { return 2; }
    };

    // Precedence 3
    class ConditionalAndExpression : public BinaryExpression {
    public:
        ConditionalAndExpression(ptr_value lhs, ptr_value rhs) : BinaryExpression(move(lhs), move(rhs)) {}
        ushort precedence() override { return 3; }
    };

    // Precedence 4
    class BitwiseOrExpression : public BinaryExpression {
    public:
        BitwiseOrExpression(ptr_value lhs, ptr_value rhs) : BinaryExpression(move(lhs), move(rhs)) {}
        ushort precedence() override { return 4; }
    };

    // Precedence 5
    class BitwiseXOrExpression : public BinaryExpression {
    public:
        BitwiseXOrExpression(ptr_value lhs, ptr_value rhs) : BinaryExpression(move(lhs), move(rhs)) {}
        ushort precedence() override { return 5; }
    };

    // Precedence 6
    class BitwiseAndExpression : public BinaryExpression {
    public:
        BitwiseAndExpression(ptr_value lhs, ptr_value rhs) : BinaryExpression(move(lhs), move(rhs)) {}
        ushort precedence() override { return 6; }
    };

    // Precedence 7
    class EqualExpression : public BinaryExpression {
    public:
        EqualExpression(ptr_value lhs, ptr_value rhs) : BinaryExpression(move(lhs), move(rhs)) {}
        ushort precedence() override { return 7; }
    };
    class NotEqualExpression : public BinaryExpression {
    public:
        NotEqualExpression(ptr_value lhs, ptr_value rhs) : BinaryExpression(move(lhs), move(rhs)) {}
        ushort precedence() override { return 7; }
    };

    // Precedence 8
    class LessThanExpression : public BinaryExpression {
    public:
        LessThanExpression(ptr_value lhs, ptr_value rhs) : BinaryExpression(move(lhs), move(rhs)) {}
        ushort precedence() override { return 8; }
    };
    class GreaterThanExpression : public BinaryExpression {
    public:
        GreaterThanExpression(ptr_value lhs, ptr_value rhs) : BinaryExpression(move(lhs), move(rhs)) {}
        ushort precedence() override { return 8; }
    };
    class LessOrEqualExpression : public BinaryExpression {
    public:
        LessOrEqualExpression(ptr_value lhs, ptr_value rhs) : BinaryExpression(move(lhs), move(rhs)) {}
        ushort precedence() override { return 8; }
    };
    class GreaterOrEqualExpression : public BinaryExpression {
    public:
        GreaterOrEqualExpression(ptr_value lhs, ptr_value rhs) : BinaryExpression(move(lhs), move(rhs)) {}
        ushort precedence() override { return 8; }
    };

    // Precedence 9
    class LeftShiftExpression : public BinaryExpression {
    public:
        LeftShiftExpression(ptr_value lhs, ptr_value rhs) : BinaryExpression(move(lhs), move(rhs)) {}
        ushort precedence() override { return 9; }
    };
    class RightShiftExpression : public BinaryExpression {
    public:
        RightShiftExpression(ptr_value lhs, ptr_value rhs) : BinaryExpression(move(lhs), move(rhs)) {}
        ushort precedence() override { return 9; }
    };
    
    // Precedence 10
    class AddExpression : public BinaryExpression {
    public:
        AddExpression(ptr_value lhs, ptr_value rhs) : BinaryExpression(move(lhs), move(rhs)) {}
        ushort precedence() override { return 10; }
    };
    class SubtractExpression : public BinaryExpression {
    public:
        SubtractExpression(ptr_value lhs, ptr_value rhs) : BinaryExpression(move(lhs), move(rhs)) {}
        ushort precedence() override { return 10; }
    };

    // Precedence 11
    class MultiplyExpression : public BinaryExpression {
    public:
        MultiplyExpression(ptr_value lhs, ptr_value rhs) : BinaryExpression(move(lhs), move(rhs)) {}
        ushort precedence() override { return 11; }
    };
    class DivideExpression : public BinaryExpression {
    public:
        DivideExpression(ptr_value lhs, ptr_value rhs) : BinaryExpression(move(lhs), move(rhs)) {}
        ushort precedence() override { return 11; }
    };
    class ReminderExpression : public BinaryExpression {
    public:
        ReminderExpression(ptr_value lhs, ptr_value rhs) : BinaryExpression(move(lhs), move(rhs)) {}
        ushort precedence() override { return 11; }
    };

    // Precedence 12
    class ConditionalNotExpression : public UnaryExpression {
    public:
        ConditionalNotExpression(ptr_value operand) : UnaryExpression(move(operand)) {}
        ushort precedence() override { return 12; }
    };
    class UnaryPlusExpression : public UnaryExpression {
    public:
        UnaryPlusExpression(ptr_value operand) : UnaryExpression(move(operand)) {}
        ushort precedence() override { return 12; }
    };
    class UnaryMinusExpression : public UnaryExpression {
    public:
        UnaryMinusExpression(ptr_value operand) : UnaryExpression(move(operand)) {}
        ushort precedence() override { return 12; }
    };
    class PreIncrementExpression : public UnaryExpression {
    public:
        PreIncrementExpression(ptr_value operand) : UnaryExpression(move(operand)) {}
        ushort precedence() override { return 12; }
    };
    class PreDecrementExpression : public UnaryExpression {
    public:
        PreDecrementExpression(ptr_value operand) : UnaryExpression(move(operand)) {}
        ushort precedence() override { return 12; }
    };
    class BitComplementExpression : public UnaryExpression {
    public:
        BitComplementExpression(ptr_value operand) : UnaryExpression(move(operand)) {}
        ushort precedence() override { return 12; }
    };

    // Precedence 13 Expressions
    class IdentifierExpression : public ValueExpression {
        Token _identifier;
    public:
        IdentifierExpression(Token identifier) : _identifier(identifier) {}
        ushort precedence() override { return 13; }
        string name() { return _identifier.value<string>(); }
    };
    class ConstantValueExpression : public ValueExpression {
        Token _literal;
    public:
        ConstantValueExpression(Token literal) : _literal(literal) {}
        ushort precedence() override { return 13; }
    };
    class MemberAccessExpression : public UnaryExpression {
        Token _name;
    public:
        MemberAccessExpression(ptr_value operand, Token name) : UnaryExpression(move(operand)), _name(name) {}
        ushort precedence() override { return 13; }
        string name() { return _name.value<string>(); }
    };
    class InvocationExpression : public UnaryExpression {
        vector<ptr_value> _values;
    public:
        InvocationExpression(ptr_value lhs, vector<ptr_value> values) : UnaryExpression(move(lhs)), _values(move(values)) {}
        ushort precedence() override { return 13; }
    };
    class PostIncrementExpression : public UnaryExpression {
    public:
        PostIncrementExpression(ptr_value operand) : UnaryExpression(move(operand)) {}
        ushort precedence() override { return 13; }
    };
    class PostDecrementExpression : public UnaryExpression {
    public:
        PostDecrementExpression(ptr_value operand) : UnaryExpression(move(operand)) {}
        ushort precedence() override { return 13; }
    };

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
        if (!is<EoF>()) {
            _lookaheadToken = *++_current;
            // Ignore whitespace.
            // TODO: Capture last comment before members for docs.
            while(is<Whitespace>()) _lookaheadToken = *++_current;
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
            if (is<Assignment>()) {
                // <Assign> ::= <Prec12Exp> assign <Expression>
                consume<Assignment>();
                lhs = make_unique<AssignmentExpression>(move(lhs), parseValueExpression());
            } else if (is<AdditionAssignment>()) {
                // <AddAssign> ::= <Prec12Exp> add-assign <Expression>
                consume<AdditionAssignment>();
                lhs = make_unique<AdditionAssignmentExpression>(move(lhs), parseValueExpression());
            } else if (is<SubtractionAssignment>()) {
                // <SubtractAssign> ::= <Prec12Exp> subtract-assign <Expression>
                consume<SubtractionAssignment>();
                lhs = make_unique<SubtractionAssignmentExpression>(move(lhs), parseValueExpression());
            } else if (is<MultiplicationAssignment>()) {
                // <MultiplyAssign> ::= <Prec12Exp> multiply-assign <Expression>
                consume<MultiplicationAssignment>();
                lhs = make_unique<MultiplicationAssignmentExpression>(move(lhs), parseValueExpression());
            } else if (is<DivisionAssignment>()) {
                // <DivideAssign> ::= <Prec12Exp> divide-assign <Expression>
                consume<DivisionAssignment>();
                lhs = make_unique<DivisionAssignmentExpression>(move(lhs), parseValueExpression());
            } else if (is<BitAndAssignment>()) {
                // <BitAndAssign> ::= <Prec12Exp> bit-and-assign <Expression>
                consume<BitAndAssignment>();
                lhs = make_unique<BitAndAssignmentExpression>(move(lhs), parseValueExpression());
            } else if (is<BitXOrAssignment>()) {
                // <BitXorAssign> ::= <Prec12Exp> bit-xor-assign <Expression>
                consume<BitXOrAssignment>();
                lhs = make_unique<BitXOrAssignmentExpression>(move(lhs), parseValueExpression());
            } else if (is<BitOrAssignment>()) {
                // <BitOrAssign> ::= <Prec12Exp> bit-or-assign <Expression>
                consume<BitOrAssignment>();
                lhs = make_unique<BitOrAssignmentExpression>(move(lhs), parseValueExpression());
            } else if (is<ReminderAssignment>()) {
                // <ReminderAssign> ::= <Prec12Exp> reminder-assign <Expression>
                consume<ReminderAssignment>();
                lhs = make_unique<ReminderAssignmentExpression>(move(lhs), parseValueExpression());
            } else if (is<LeftShiftAssignment>()) {
                // <ShiftLeftAssign> ::= <Prec12Exp> shift-left-assign <Expression>
                consume<LeftShiftAssignment>();
                lhs = make_unique<LeftShiftAssignmentExpression>(move(lhs), parseValueExpression());
            } else if (is<RightShiftAssignment>()) {
                // <ShiftRightAssign> ::= <Prec12Exp> shift-right-assign <Expression>
                consume<RightShiftAssignment>();
                lhs = make_unique<RightShiftAssignmentExpression>(move(lhs), parseValueExpression());
            }
        }
        return lhs;
    }

    ptr_value parsePrec1ValueExpression() {
        // ! Conditional If Operators
        // <Prec1Exp> ::= <ConditionalIf> | <Prec2Exp>
        // <ConditionalIf> ::= <Prec2Exp> question <Expression> colon <Expression>
        ptr_value conditionExpression = parsePrec2ValueExpression();
        if (!is<Question>()) return conditionExpression;

        consume<Question>();
        ptr_value thenExpression = parseValueExpression();
        consume<Colon>();
        ptr_value elseExpression = parseValueExpression();
        return make_unique<ConditionalIfExpression>(move(conditionExpression), move(thenExpression), move(elseExpression));
    }

    ptr_value parsePrec2ValueExpression() {
        // ! Conditional Operators
        // <Prec2Exp> ::= <Prec3Exp> | <ConditionalOr>
        // <ConditionalOr> ::= <Prec2Exp> conditional-or <Prec3Exp>       
        ptr_value lhs = parsePrec3ValueExpression();
        while(is<ConditionalOr>()) {
            consume<ConditionalOr>();
            ptr_value rhs = parsePrec3ValueExpression();
            lhs = make_unique<ConditionalOrExpression>(move(lhs), move(rhs));
        }
        return lhs;
    }

    ptr_value parsePrec3ValueExpression() {
        // ! Conditional Operators
        // <Prec3Exp> ::= <Prec4Exp> | <ConditionalAnd>
        // <ConditionalAnd> ::=  <Prec3Exp> conditional-and <Prec4Exp>
        ptr_value lhs = parsePrec4ValueExpression();
        while(is<ConditionalAnd>()) {
            consume<ConditionalAnd>();
            ptr_value rhs = parsePrec4ValueExpression();
            lhs = make_unique<ConditionalAndExpression>(move(lhs), move(rhs));
        }
        return lhs;
    }

    ptr_value parsePrec4ValueExpression() {
        // ! Bitwise Operators
        // <Prec4Exp> ::= <Prec5Exp> | <BitwiseOr>
        // <BitwiseOr> ::= <Prec4Exp> bitwise-or <Prec5Exp>
        ptr_value lhs = parsePrec5ValueExpression();
        while(is<BitwiseOr>()) {
            consume<BitwiseOr>();
            ptr_value rhs = parsePrec5ValueExpression();
            lhs = make_unique<BitwiseOrExpression>(move(lhs), move(rhs));
        }
        return lhs;
    }

    ptr_value parsePrec5ValueExpression() {
        // <Prec5Exp> ::= <Prec6Exp> | <BitwiseXOr>
        // <BitwiseXOr> ::= <Prec5Exp> bitwise-xor <Prec6Exp>
        ptr_value lhs = parsePrec6ValueExpression();
        while(is<BitwiseXOr>()) {
            consume<BitwiseXOr>();
            ptr_value rhs = parsePrec6ValueExpression();
            lhs = make_unique<BitwiseXOrExpression>(move(lhs), move(rhs));
        }
        return lhs;
    }

    ptr_value parsePrec6ValueExpression() {
        // <Prec6Exp> ::= <Prec7Exp> | <BitwiseAnd>
        // <BitwiseAnd> ::= <Prec6Exp> bitwise-and <Prec7Exp>
        ptr_value lhs = parsePrec7ValueExpression();
        while(is<BitwiseAnd>()) {
            consume<BitwiseAnd>();
            ptr_value rhs = parsePrec7ValueExpression();
            lhs = make_unique<BitwiseAndExpression>(move(lhs), move(rhs));
        }
        return lhs;
    }

    ptr_value parsePrec7ValueExpression() {
        // ! Equality Operators
        // <Prec7Exp> ::= <Prec8Exp> | <Equal> | <NotEqual>
        ptr_value lhs = parsePrec8ValueExpression();
        do {
            if (is<Equal>()) {
                // <Equal> ::= <Prec7Exp> equal <Prec8Exp>
                consume<Equal>();
                ptr_value rhs = parsePrec8ValueExpression();
                lhs = make_unique<EqualExpression>(move(lhs), move(rhs));
            } else if (is<NotEqual>()) {
                // <NotEqual> ::= <Prec7Exp> not-equal <Prec8Exp>
                consume<NotEqual>();
                ptr_value rhs = parsePrec8ValueExpression();
                lhs = make_unique<NotEqualExpression>(move(lhs), move(rhs));
            } else break;
        } while(true);
        return lhs;
    }

    ptr_value parsePrec8ValueExpression() {
        // ! Comparison Operators
        // <Prec8Exp> ::= <Prec9Exp> | <LessThan> | <GreaterThan> | <LessOrEqual> | <GreaterOrEqual>
        ptr_value lhs = parsePrec9ValueExpression();
        do {
            if (is<Less>()) {
                // <LessThan> ::= <Prec8Exp> less-than <Prec9Exp>
                consume<Less>();
                ptr_value rhs = parsePrec9ValueExpression();
                lhs = make_unique<LessThanExpression>(move(lhs), move(rhs));
            } else if (is<Greater>()) {
                // <GreaterThan> ::= <Prec8Exp> greater-than <Prec9Exp>
                consume<Greater>();
                ptr_value rhs = parsePrec9ValueExpression();
                lhs = make_unique<GreaterThanExpression>(move(lhs), move(rhs));
            } else if (is<LessOrEqual>()) {
                // <LessOrEqual> ::= <Prec8Exp> less-or-equal <Prec9Exp>
                consume<LessOrEqual>();
                ptr_value rhs = parsePrec9ValueExpression();
                lhs = make_unique<LessOrEqualExpression>(move(lhs), move(rhs));
            } else if (is<GreaterOrEqual>()) {
                // <GreaterOrEqual> ::= <Prec8Exp> greater-or-equal <Prec9Exp>
                consume<GreaterOrEqual>();
                ptr_value rhs = parsePrec9ValueExpression();
                lhs = make_unique<GreaterOrEqualExpression>(move(lhs), move(rhs));
            } else break;
        } while(true);
        return lhs;
    }

    ptr_value parsePrec9ValueExpression() {
        // ! Shift Operators
        // <Prec9Exp> ::= <Prec10Exp> | <LeftShift> | <RightShift>
        ptr_value lhs = parsePrec10ValueExpression();

        do {
            if (is<LeftShift>()) {
                // <LeftShift> ::= <Prec9Exp> shift-left <Prec10Exp>
                consume<LeftShift>();
                ptr_value rhs = parsePrec10ValueExpression();
                lhs = make_unique<LeftShiftExpression>(move(lhs), move(rhs));
            } else if (is<RightShift>()) {
                // <RightShift> ::= <Prec9Exp> shift-right <Prec10Exp>
                consume<RightShift>();
                ptr_value rhs = parsePrec10ValueExpression();
                lhs = make_unique<RightShiftExpression>(move(lhs), move(rhs));
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
            if (is<Plus>()) {
                // <Add> ::= <Prec10Exp> plus <Prec11Exp>
                consume<Plus>();
                ptr_value rhs = parsePrec11ValueExpression();
                prec10Exp = make_unique<AddExpression>(move(prec10Exp), move(rhs));
            } else if (is<Minus>()) {
                // <Subtract> ::= <Prec10Exp> minus <Prec11Exp>
                consume<Minus>();
                ptr_value rhs = parsePrec11ValueExpression();
                prec10Exp = make_unique<SubtractExpression>(move(prec10Exp), move(rhs));
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
            if (is<Multiply>()) {
                // <Multiply> ::= <Prec11Exp> multiply <Prec12Exp>
                consume<Multiply>();
                ptr_value rhs = parsePrec12ValueExpression();
                prec11Exp = make_unique<MultiplyExpression>(move(prec11Exp), move(rhs));
            } else if (is<Divide>()) {
                // <Divide> ::= <Prec11Exp> divide <Prec12Exp>
                consume<Divide>();
                ptr_value rhs = parsePrec12ValueExpression();
                prec11Exp = make_unique<DivideExpression>(move(prec11Exp), move(rhs));
            } else if (is<Reminder>()) {
                // <Reminder> ::= <Prec11Exp> reminder <Prec12Exp>
                consume<Reminder>();
                ptr_value rhs = parsePrec12ValueExpression();
                prec11Exp = make_unique<ReminderExpression>(move(prec11Exp), move(rhs));
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

        if (is<ConditionalNot>()) return make_unique<ConditionalNotExpression>(parsePrec12ValueExpression());
        else if (is<Plus>()) return make_unique<UnaryPlusExpression>(parsePrec12ValueExpression());
        else if (is<Minus>()) return make_unique<UnaryMinusExpression>(parsePrec12ValueExpression());
        else if (is<Increment>()) return make_unique<PreIncrementExpression>(parsePrec12ValueExpression());
        else if (is<Decrement>()) return make_unique<PreDecrementExpression>(parsePrec12ValueExpression());
        else if (is<BitComplement>()) return make_unique<BitComplementExpression>(parsePrec12ValueExpression());
        else return parsePrec13ValueExpression();
    }

    ptr_value parsePrec13ValueExpression() {
        ptr_value prec13Exp = nullptr;
        // Precedence 13 operators: Unary Post-Operators and Base Expressions
        // <Prec13Exp> ::=
        // <Identifier>
        if (is<Identifier>()) prec13Exp = make_unique<IdentifierExpression>(consume<Identifier>());
        // <ConstantValue>
        else if (is<TokenType::Float>()) prec13Exp = make_unique<ConstantValueExpression>(consume<TokenType::Float>());
        else if (is<TokenType::Integer>()) prec13Exp = make_unique<ConstantValueExpression>(consume<TokenType::Integer>());
        else if (is<SingleQuotesString>()) prec13Exp = make_unique<ConstantValueExpression>(consume<SingleQuotesString>());
        else if (is<DoubleQuotesString>()) prec13Exp = make_unique<ConstantValueExpression>(consume<DoubleQuotesString>());
        // <ParenExpression>
        else if (is<OpenParenthesis>()) {
            consume<OpenParenthesis>();
            prec13Exp = parseValueExpression();
            consume<CloseParenthesis>();
        }
        else throw CompilerError(UnexpectedParserEoF, span());

        do {
            // <MemberAccess> ::= <Prec13Exp> dot <Member>
            if (is<Dot>()) {
                consume<Dot>();
                // <Member> ::= Identifier
                auto name = consume<Identifier>();
                prec13Exp = make_unique<MemberAccessExpression>(move(prec13Exp), name);
            } else if (is<OpenParenthesis>()) {
                // <Invocation> ::= <Prec13Exp> l-paren <Values> r-paren
                consume<OpenParenthesis>();
                // <Values> ::= <Expression> comma <Values> | <Expression> | <>
                vector<ptr_value> values;
                if (!is<CloseParenthesis>()) {
                    do {
                        values.push_back(parseValueExpression());
                        if (is<Comma>()) consume<Comma>();
                        else break;
                    } while(true);
                }
                consume<CloseParenthesis>();
                prec13Exp = make_unique<InvocationExpression>(move(prec13Exp), move(values));
            } else if (is<Increment>()) {
                // <PostIncrement> ::= <Prec13Exp> increment
                consume<Increment>();
                prec13Exp = make_unique<PostIncrementExpression>(move(prec13Exp));
            } else if (is<Decrement>()) {
                // <PostDecrement> ::= <Prec13Exp> decrement
                consume<Decrement>();
                prec13Exp = make_unique<PostDecrementExpression>(move(prec13Exp));
            } else break;
        } while(true);
        return prec13Exp;
    }

    unique_ptr<ConstStatement> parseConstStatement() {
        consume<ConstKeyword>();
        auto name = consume<Identifier>();

        ptr_type type = nullptr;
        if (is<Colon>()) {
            consume<Colon>();
            type = parseTypeExpression();
        }

        consume<Assignment>();
        auto value = parseValueExpression();
        consume<Semicolon>();
        return make_unique<ConstStatement>(name, move(type), move(value));
    }

    unique_ptr<ReturnStatement> parseReturnStatement() {
        consume<ReturnKeyword>();
        ptr_value value = nullptr;
        if (!is<Semicolon>()) value = parseValueExpression();
        consume<Semicolon>();
        return make_unique<ReturnStatement>(move(value));
    }

    ptr_statement parseStatement() {
        if (is<ConstKeyword>()) return parseConstStatement();
        if (is<ReturnKeyword>()) return parseReturnStatement();

        // TODO: for, if, etc.

        ptr_statement expression = parseValueExpression();
        consume<Semicolon>();
        return expression;
    }

    unique_ptr<CodeBlock> parseCodeBlock() {
        consume<LeftBrace>();
        vector<ptr_statement> _statements;
        while(!is<RightBrace>()) {
            if (is<EoF>()) throw CompilerError(UnexpectedParserEoF, span());
            _statements.push_back(parseStatement());
        }
        consume<RightBrace>();
        return make_unique<CodeBlock>(move(_statements));
    }

    ptr_type parseTypeExpression() {
        if (is<IntKeyword>()) {
            return make_unique<PrimitiveTypeExpression>(consume<IntKeyword>(), PrimitiveType::Int);
        } else if (is<FloatKeyword>()) {
            return make_unique<PrimitiveTypeExpression>(consume<FloatKeyword>(), PrimitiveType::Float);
        } else if (is<StringKeyword>()) {
            return make_unique<PrimitiveTypeExpression>(consume<StringKeyword>(), PrimitiveType::String);
        } else if (is<BoolKeyword>()) {
            return make_unique<PrimitiveTypeExpression>(consume<BoolKeyword>(), PrimitiveType::Bool);
        }

        // TODO: Identifier, fully qualified names, generics, etc.
        throw CompilerError(UnexpectedToken, span());
    }

    unique_ptr<FunctionArgument> consumeFunctionArgument() {
        Token identifier;
        if (is<Identifier>()) {
            identifier = consume<Identifier>();
        }
        consume<Colon>();
        ptr_type type = parseTypeExpression();

        return make_unique<FunctionArgument>(identifier, move(type));
    }

    unique_ptr<FunctionPrototype> consumeFunctionPrototype() {
        vector<unique_ptr<FunctionArgument> > args;

        consume<OpenParenthesis>();
        if (!is<CloseParenthesis>()) {
            args.push_back(consumeFunctionArgument());
            while(is<Comma>()) {
                consume<Comma>();
                args.push_back(consumeFunctionArgument());
            }
        }
        consume<CloseParenthesis>();

        ptr_type returnTypeAnnotation = nullptr;
        if (is<Colon>()) {
            consume<Colon>();
            returnTypeAnnotation = parseTypeExpression();
        }

        return make_unique<FunctionPrototype>(move(args), move(returnTypeAnnotation));
    }

    unique_ptr<Function> consumeFunction() {
        auto functionKeywordToken = consume<FunctionKeyword>();
        auto identifier = consume<Identifier>();
        auto proto = consumeFunctionPrototype();
        auto block = parseCodeBlock();

        // TODO: Add args, add return type, add code block.
        return make_unique<Function>(identifier, move(proto), move(block));
    }

    unique_ptr<FunctionDeclaration> consumeFunctionDeclaration(Token& declareKeyword) {
        auto functionKeywordToken = consume<FunctionKeyword>();
        auto identifier = consume<Identifier>();
        auto proto = consumeFunctionPrototype();
        auto semicolon = consume<Semicolon>();

        return make_unique<FunctionDeclaration>(identifier, move(proto));
    }

    void parseModuleMembers() {
        while(!is<EoF>()) {
            while(is<Whitespace>()) consume<Whitespace>();
            if (is<FunctionKeyword>()) {
                _members.push_back(consumeFunction());
            } else if (is<DeclareKeyword>()) {
                Token declareKeyword = consume<DeclareKeyword>();
                if (is<FunctionKeyword>()) {
                    _members.push_back(consumeFunctionDeclaration(declareKeyword));
                } else {
                    throw CompilerError(UnexpectedToken, span());
                }
            }
            else throw CompilerError(UnexpectedToken, span());
        }
    }
};

/// EXPLORATION TESTS ///

void printTokens(string program) {
    Lexer lexer(program);
    try {
        for (auto token : lexer) {
            if (token.type() != Whitespace) {
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

class ASTPrinter : public Parser::ModuleMemberVisitor, public Parser::TypeExpressionVisitor {
protected:
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
        // TODO: Statement block visitor.
        cout << ";" << endl;
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
protected:
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

    // // ExpressionVisitor, on float
    // void on(Parser::Float) {
    //     ConstantFP::get(TheContext, APFloat(15.0));
    // }
};

void testSimpleProgramAST1() {
    string program = ""
        "declare function sin(angle: float): float;\r\n"s
        "declare function cos(angle: float): float;\r\n"s
        "function main(): float {\r\n"s
        "    const x = sin(15.0) + cos(15.0);"s
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
