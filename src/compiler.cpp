#include <iostream>
#include <vector>
#include <map>
#include <cassert>
#include <exception>
#include <variant>
#include <experimental/memory_resource>

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
    // TODO: This list will be huge!
    VoidKeyword, // void
    NeverKeyword, // never
    BoolKeyword, // bool
    IntKeyword, // int
    FloatKeyword, // float
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
    "VoidKeyword", // void
    "NeverKeyword", // never
    "BoolKeyword", // bool
    "IntKeyword", // int
    "FloatKeyword", // float
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
    { "void"s, VoidKeyword },
    { "never"s, NeverKeyword },
    { "bool"s, BoolKeyword },
    { "int"s, IntKeyword },
    { "float"s, FloatKeyword },
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
    UnexpectedEoF = 1001,
    UnexpectedCharacter = 1002,
    ExpectedADigit = 1003,
    NumberOverflow = 1004,
};

map<ErrorCode, string> errorCodeDescription = {
    { UnexpectedEoF, "Lexer error, unexpected end of file."s },
    { UnexpectedCharacter, "Lexed error, unexpected character."s },
    { ExpectedADigit, "Lexed error, expected a digit from 0 to 9."s },
    { NumberOverflow, "Lexed error, number too large."s },
};

class CompilerError : public exception {
    ErrorCode _code;
    unsigned int _line;
    unsigned int _column;

    string _message;

public:
    CompilerError(ErrorCode code, unsigned int line, unsigned int column) :
        _code(code),
        _line(line),
        _column(column),
        _message(errorCodeDescription[code] + " ("s + to_string(line) + ":" + to_string(column) + ")") {
    }

    unsigned int line() { return _line; }
    unsigned int column() { return _column; }
    virtual const char* what() const throw() { return _message.c_str(); }
};

class Lexer {
    typedef typename string::iterator StringIterator;
    typedef typename string::value_type Char;
    string _code;

public:
    Lexer(string code): _code(code) {}

    class TokenIterator;
    class Token;

    class Token {
        TokenType _type;

        unsigned int _line;
        unsigned int _column;

        StringIterator _codeStart;
        StringIterator _codeEnd;

        variant<unsigned long long, double, string> _value;

    public:
        friend class TokenIterator;

        Token() : _type(TokenType::EoF), _value(0ULL) {}

        const TokenType& type() const { return _type; }

        template<typename T>
        T value() {
            return get<T>(_value);
        }
        
        unsigned int line() const { return _line; }
        unsigned int column() const { return _column; }
        string code() const { return string(_codeStart, _codeEnd); }
        
        bool operator == (const TokenType type) const { return _type == type; }
        bool operator != (const TokenType type) const { return _type != type; }
        operator bool() const { return _type != EoF; }
        operator TokenType() const { return _type; }
    };

    class TokenIterator {
        StringIterator _current;
        StringIterator _end;
        Token _token;

        unsigned int _line;
        unsigned int _column;

    public:
        typedef input_iterator_tag iterator_category;

        TokenIterator(StringIterator begin, StringIterator end) :
            _current(begin),
            _end(end),
            _line(1),
            _column(1) {

            next();
        }

        TokenIterator& operator++() {
            next();
            return *this;
        }

        // TokenIterator operator++(int) {
        //     TokenIterator result(*this);
        //     ++(*this);
        //     return result;
        // }

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
        bool isChar(const Char& c) {
            return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c > 127);
        }
        
        bool isNumber(const Char& c) {
            return c >= '0' && c <= '9';
        }
        
        bool isWhitespace(const Char& c) {
            return c == ' ' || c == '\t' || c == '\n' || c == '\r';
        }
        
        bool isIdentifierLeadChar(const Char& c) {
            return isChar(c) || c == '_';
        }
        
        bool isIdentifierTailChar(const Char& c) {
            return isIdentifierLeadChar(c) || isNumber(c);
        }

        void consumeIdentifier(Char c) {
            string identifier;
            identifier += c;

            ++_column;
            do {
                if (_current == _end) break;
                c = *_current;
                if (!isIdentifierTailChar(c)) break;
                identifier += c;
                ++_column;
                ++_current;
            } while(true);

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
            ++_column;
            _token._type = type;
            _token._value = 0ULL;
        }

        void consumeWhitespace(Char c) {
            // String whitespace;
            do {
                // whitespace += c;

                if (_current == _end) break;
                
                // NOTE: New line combinations: \r\n or \n.
                if (c == '\n') {
                    ++_line;
                    _column = 1;
                } else {
                    ++_column;
                }

                c = *_current;
                if (!isWhitespace(c)) break;

                ++_current;
            } while(true);
            _token._type = TokenType::Whitespace;
            _token._value = 0ULL;
        }

        void consumeString(Char lead) {            
            string value;
            do {
                if (_current == _end) {
                    throw CompilerError(UnexpectedEoF, _line, _column);
                }
                const Char c = *_current;
                if (c == '\r' || c == '\n') {
                    throw CompilerError(UnexpectedCharacter, _line, _column);
                }
                if (c == '\\') {
                    // TODO: Handle escape sequences...
                    assert(false);
                }
                ++_column;
                ++_current;
                if (c == lead) {
                    _token._value = value;
                    return;
                }
                value += c;
            } while(true);
        }

        void consumeDoubleQuotesString(Char lead) {
            _token._type = TokenType::DoubleQuotesString;
            consumeString(lead);
        }

        void consumeSingleQuotesString(Char lead) {
            _token._type = TokenType::SingleQuotesString;
            consumeString(lead);
        }

        void consumeNumber(Char lead) {
            ++_column;

            string number;
            number += lead;

            bool hasFrac = false;
            bool hasExp = false;

            if (lead == '0') {
                // lead is 0
                if (_current != _end) {
                    Char c = *_current;
                    if (isNumber(c)) {
                        throw CompilerError(UnexpectedCharacter, _line, _column);
                    }
                }
            }

            while (_current != _end) {
                Char c = *_current;
                if (isNumber(c)) {
                    number += c;
                    ++_current;
                    ++_column;
                } else {
                    break;
                }
            }

            if (_current != _end) {
                Char c = *_current;
                if (c == '.') {
                    number += c;
                    ++_current;
                    ++_column;
                    hasFrac = true;

                    if (_current == _end) {
                        throw CompilerError(UnexpectedEoF, _line, _column);
                    } else if (!isNumber(*_current)) {
                        throw CompilerError(ExpectedADigit, _line, _column);
                    }

                    while (_current != _end) {
                        Char c = *_current;
                        if (isNumber(c)) {
                            number += c;
                            ++_current;
                            ++_column;
                        } else {
                            break;
                        }
                    }
                }
            }

            if (_current != _end) {
                Char c = *_current;
                if (c == 'e' || c == 'E') {
                    number += c;
                    ++_current;
                    ++_column;
                    hasExp = true;

                    if (_current == _end) {
                        throw CompilerError(UnexpectedEoF, _line, _column);
                    }

                    Char sign = *_current;
                    if (sign == '+' || sign == '-') {
                        number += sign;
                        ++_current;
                        ++_column;

                        if (_current == _end) {
                            throw CompilerError(UnexpectedEoF, _line, _column);
                        }
                    }
                    
                    if (!isNumber(*_current)) {
                        throw CompilerError(ExpectedADigit, _line, _column);
                    }

                    while (_current != _end) {
                        Char c = *_current;
                        if (isNumber(c)) {
                            number += c;
                            ++_current;
                            ++_column;
                        } else {
                            break;
                        }
                    }
                }
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
                throw CompilerError(NumberOverflow, _token._line, _token._column);
            } catch(invalid_argument e) {
                assert(false); // Numbers should be parsable by C++ std::stod() or std::stoull()
            }
        }

        void next() {
            if (_current == _end) {
                _token._type = TokenType::EoF;
                _token._value = 0ULL;
                return;
            }

            _token._line = _line;
            _token._column = _column;
            _token._codeStart = _current;

            Char lead = *_current++;
            if (isWhitespace(lead)) {
                consumeWhitespace(lead);
            } else if (isIdentifierLeadChar(lead)) {
                consumeIdentifier(lead);
            } else if (lead == '"') {
                consumeDoubleQuotesString(lead);
            } else if (lead == '\'') {
                consumeSingleQuotesString(lead);
            } else if (isNumber(lead)) {
                consumeNumber(lead);
            } else {
                // consume operator
                string op;
                op += lead;

                auto found = operatorSymbols.find(op);
                if (found != operatorSymbols.end()) {
                    do {
                        if (_current == _end) {
                            break;
                        }
                        op += *_current;
                        auto next = operatorSymbols.find(op);
                        if (next == operatorSymbols.end()) {
                            break;
                        }
                        found = next;
                        _current++;
                        _column++;
                    } while(true);

                    _token._type = found->second;
                    _token._value = 0ULL;
                } else {
                    throw CompilerError(UnexpectedCharacter, _line, _column);
                }
            }

            _token._codeEnd = _current;
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
    out << name(token.type()) << " (" << token.line() << ":" << token.column() << ") ";
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

int main(int argl, char** argv) {
    // cout << "testSimpleProgram1" << endl;
    // testSimpleProgram1();
    // cout << "testSimpleProgram2" << endl;
    // testSimpleProgram2();
    // cout << "testSimpleProgram3" << endl;
    // testSimpleProgram3();

    std::experimental::pmr::monotonic_buffer_resource resource;
    // std::experimental::pmr::string s(&ressource);
    // s += "asdasd"s;
    // s += "asdasd"s;
    // cout << s;
}
