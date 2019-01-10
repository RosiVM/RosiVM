#include <iostream>
#include <map>
#include <cassert>

#include "lex.h"

using namespace std;
using namespace rvm;

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

string rvm::name(TokenType type) {
    return tokenTypeNames[static_cast<unsigned int>(type)];
}

const map<string, TokenType> rvm::keywords {
    { "declare", TokenType::DeclareKeyword },
    { "function", TokenType::FunctionKeyword },
    { "void"s, TokenType::VoidKeyword },
    { "never"s, TokenType::NeverKeyword },
    { "bool"s, TokenType::BoolKeyword },
    { "int"s, TokenType::IntKeyword },
    { "float"s, TokenType::FloatKeyword },
    { "string"s, TokenType::StringKeyword },
    { "var"s, TokenType::VarKeyword },
    { "const"s, TokenType::ConstKeyword },
    { "return"s, TokenType::ReturnKeyword },
};

const map<string, TokenType> rvm::operatorSymbols {
    { "("s, TokenType::OpenParenthesis },
    { ")"s, TokenType::CloseParenthesis },
    { "["s, TokenType::LeftBracket },
    { "]"s, TokenType::RightBracket },
    { "{"s, TokenType::LeftBrace },
    { "}"s, TokenType::RightBrace },
    { ":"s, TokenType::Colon },
    { ";"s, TokenType::Semicolon },
    { ","s, TokenType::Comma },
    { "."s, TokenType::Dot },

    { "="s, TokenType::Assignment },
    { "=="s, TokenType::Equal },
    { "<"s, TokenType::Less },
    { "<="s, TokenType::LessOrEqual },
    { ">"s, TokenType::Greater },
    { ">="s, TokenType::GreaterOrEqual },
    { "!="s, TokenType::NotEqual },

    { "+"s, TokenType::Plus },
    { "-"s, TokenType::Minus },
    { "*"s, TokenType::Multiply },
    { "/"s, TokenType::Divide },
    { "%"s, TokenType::Reminder },

    { "&"s, TokenType::BitwiseAnd },
    { "|"s, TokenType::BitwiseOr },
    { "^"s, TokenType::BitwiseXOr },

    { "?"s, TokenType::Question },
    { "~"s, TokenType::BitComplement },
    
    { "++"s, TokenType::Increment },
    { "--"s, TokenType::Decrement },

    { "&&"s, TokenType::ConditionalAnd },
    { "||"s, TokenType::ConditionalOr },
    { "!"s, TokenType::ConditionalNot },
    
    { "<<"s, TokenType::LeftShift },
    { ">>"s, TokenType::RightShift },

    { "+="s, TokenType::AdditionAssignment },
    { "-="s, TokenType::SubtractionAssignment },
    { "*="s, TokenType::MultiplicationAssignment },
    { "/="s, TokenType::DivisionAssignment },
    { "%="s, TokenType::ReminderAssignment },

    { "&="s, TokenType::BitAndAssignment },
    { "|="s, TokenType::BitOrAssignment },
    { "^="s, TokenType::BitXOrAssignment },
    
    { "<<="s, TokenType::LeftShiftAssignment },
    { ">>="s, TokenType::RightShiftAssignment },

    { "->", TokenType::PointerMemberAccess },
    { "??", TokenType::NullCoalescing },
    { "=>", TokenType::LambdaOperator },
};

Lexer::TokenIterator Lexer::begin() {
    return TokenIterator(_code.begin(), _code.end());
}
Lexer::TokenIterator Lexer::end() {
    StringIterator end = _code.end();
    return TokenIterator(end, end);
}

Lexer::TokenIterator::TokenIterator(StringIterator begin, StringIterator end) :
    _current(begin),
    _end(end),
    _lookaheadChar(_current == _end ? 0 : *_current) {

    consumeNext();
}

Lexer::TokenIterator& Lexer::TokenIterator::operator++() {
    consumeNext();
    return *this;
}

Lexer::TokenIterator Lexer::TokenIterator::operator++(int) {
    TokenIterator result(*this);
    ++(*this);
    return result;
}

Char Lexer::TokenIterator::consumeChar() {
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

void Lexer::TokenIterator::consumeIdentifier() {
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

void Lexer::TokenIterator::consumeSingleCharToken(const Char& c, const TokenType& type) {
    ++_point.column;
    _token._type = type;
    _token._value = 0ULL;
}

void Lexer::TokenIterator::consumeWhitespace() {
    while(isWhitespace()) consumeChar();
    _token._type = TokenType::Whitespace;
    _token._value = 0ULL;
}

void Lexer::TokenIterator::consumeString() {
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

void Lexer::TokenIterator::consumeDoubleQuotesString() {
    assert(_lookaheadChar == '"');
    _token._type = TokenType::DoubleQuotesString;
    consumeString();
}

void Lexer::TokenIterator::consumeSingleQuotesString() {
    assert(_lookaheadChar == '\'');
    _token._type = TokenType::SingleQuotesString;
    consumeString();
}

void Lexer::TokenIterator::consumeNumber() {
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
            _token._type = TokenType::Float;
        } else {
            _token._value = stoull(number);
            _token._type = TokenType::Integer;
        }
    } catch(out_of_range e) {
        throw CompilerError(NumberOverflow, _point);
    } catch(invalid_argument e) {
        assert(false); // Numbers should be parsable by C++ std::stod() or std::stoull()
    }
}

void Lexer::TokenIterator::consumeOperator() {
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

void Lexer::TokenIterator::consumeNext() {
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

bool Lexer::TokenIterator::operator== (const Lexer::TokenIterator& other) {
    return _end == other._end
        && _current == other._current
        && _token._type == other._token._type;
}

bool Lexer::TokenIterator::operator!= (const Lexer::TokenIterator& other) {
    return _end != other._end
        || _current != other._current
        || _token._type != other._token._type;
}

ostream& operator << (ostream& out, Lexer::Token& token) {
    out << name(token.type()) << " (" << token.span() << ") ";
    switch(token.type()) {
        case TokenType::Identifier:
            out << token.value<string>() << " ";
            break;
        case TokenType::SingleQuotesString:
            out << '\'' << token.value<string>() << '\'' << " ";
            break;
        case TokenType::DoubleQuotesString:
            out << '"' << token.value<string>() << '"' << " ";
            break;
        case TokenType::Integer:
            out << token.value<unsigned long long int>() << " ";
            break;
        case TokenType::Float:
            out << token.value<double>() << " ";
            break;
        default:
            break;
    }

    return out;
}

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
