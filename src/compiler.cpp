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
    Tilde, // ~
    
    Increment, // ++
    Decrement, // --

    LogicalAnd, // &&
    LogicalOr, // ||
    LogicalNot, // !
    
    LeftShift, // <<
    RightShift, // >>

    AdditionAssignment, // +=
    SubtractionAssignment, // -=
    MultiplicationAssignment, // *=
    DivisionAssignment, // /=
    ReminderAssignment, // %=

    AndAssignment, // &=
    OrAssignment, // |=
    XOrAssignment, // ^=
    
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
    "Tilde", // ~
    
    "Increment", // ++
    "Decrement", // --

    "LogicalAnd", // &&
    "LogicalOr", // ||
    "LogicalNot", // !
    
    "LeftShift", // <<
    "RightShift", // >>

    "AdditionAssignment", // +=
    "SubtractionAssignment", // -=
    "MultiplicationAssignment", // *=
    "DivisionAssignment", // /=
    "ReminderAssignment", // %=

    "AndAssignment", // &=
    "OrAssignment", // |=
    "XOrAssignment", // ^=
    
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
    { "~"s, Tilde },
    
    { "++"s, Increment },
    { "--"s, Decrement },

    { "&&"s, LogicalAnd },
    { "||"s, LogicalOr },
    { "!"s, LogicalNot },
    
    { "<<"s, LeftShift },
    { ">>"s, RightShift },

    { "+="s, AdditionAssignment },
    { "-="s, SubtractionAssignment },
    { "*="s, MultiplicationAssignment },
    { "/="s, DivisionAssignment },
    { "%="s, ReminderAssignment },

    { "&="s, AndAssignment },
    { "|="s, OrAssignment },
    { "^="s, XOrAssignment },
    
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
    UnexpectedToken = 2001,
    ExpectedIdentifier = 2002,
};

map<ErrorCode, string> errorCodeDescription = {
    // Lexer errors
    { UnexpectedEoF, "Lexer error, unexpected end of file."s },
    { UnexpectedCharacter, "Lexed error, unexpected character."s },
    { ExpectedADigit, "Lexed error, expected a digit from 0 to 9."s },
    { NumberOverflow, "Lexed error, number too large."s },

    // Parser errors
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

    class CodeBlock;
    class TypeExpression;
    class PrimitiveTypeExpression;

    class ModuleMemberVisitor;
    class TypeExpressionVisitor;

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
        unique_ptr<TypeExpression>& returnType() { return _proto->returnType(); }
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
        unique_ptr<TypeExpression>& returnType() { return _proto->returnType(); }
        unique_ptr<FunctionPrototype>& proto() { return _proto; }

        void visit(ModuleMemberVisitor* visitor) override { visitor->on(this); }
    };

    class FunctionArgument {
        Token _identifier;
        unique_ptr<TypeExpression> _type;
    public:
        FunctionArgument(Token identifier, unique_ptr<TypeExpression> type) : _identifier(identifier), _type(move(type)) {}

        string name() { return _identifier.value<string>(); }
        unique_ptr<TypeExpression>& type() { return _type; }
    };

    class FunctionPrototype {
        vector<unique_ptr<FunctionArgument> > _args;
        unique_ptr<TypeExpression> _returnType;
    public:
        FunctionPrototype(vector<unique_ptr<FunctionArgument> > args, unique_ptr<TypeExpression> returnType) :
            _args(move(args)),
            _returnType(move(returnType)) {}
        
        vector<unique_ptr<FunctionArgument> >& args() { return _args; }
        unique_ptr<TypeExpression>& returnType() { return _returnType; }
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

    class CodeBlock {};

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

    unique_ptr<CodeBlock> parseCodeBlock() {
        consume<LeftBrace>();
        // TODO: Parse statements.
        consume<RightBrace>();
        return make_unique<CodeBlock>();
    }

    unique_ptr<TypeExpression> consumeTypeExpression() {
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
        unique_ptr<TypeExpression> type = consumeTypeExpression();

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

        unique_ptr<TypeExpression> returnTypeAnnotation = nullptr;
        if (is<Colon>()) {
            consume<Colon>();
            returnTypeAnnotation = consumeTypeExpression();
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
        "function main(argc: int, argv: string): int {}"s;

    // Lexer lexer(program);
    // printTokens(program);
    // cout << "====================" << endl;

    Parser parser(program);
    parser.parseModule();

    ASTPrinter printer;
    parser.visit(&printer);

    // TODO: Typechecking!

    TheModule = llvm::make_unique<Module>("my cool jit", TheContext);
    LLIRCompiler llirCompiler;
    parser.visit(&llirCompiler);
    TheModule->print(errs(), nullptr);

    // TODO: Make a fucking LLVM compiler now!
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
