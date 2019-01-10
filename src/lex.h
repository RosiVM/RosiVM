#ifndef RVM_LEX_H
#define RVM_LEX_H

#include <string>
#include <variant>
#include "src.h"

namespace rvm {
    enum class TokenType : unsigned int {
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

    std::string name(TokenType type);

    extern const std::map<std::string, TokenType> keywords;
    extern const std::map<std::string, TokenType> operatorSymbols;

    class Lexer {
        std::string _code;

    public:
        Lexer(std::string code): _code(code) {}

        class TokenIterator;
        class Token;

        class Token {
            TokenType _type;
            std::variant<unsigned long long, double, std::string> _value;

            SourceSpan _sourceSpan;
            StringSpan _stringSpan;

        public:
            friend class TokenIterator;

            Token() : _type(TokenType::EoF), _value(0ULL) {}

            const TokenType& type() const { return _type; }

            template<typename T>
            T value() { return std::get<T>(_value); }

            const SourceSpan span() const { return _sourceSpan; }
            std::string code() const { return _stringSpan.toString(); }
            
            bool operator == (const TokenType type) const { return _type == type; }
            bool operator != (const TokenType type) const { return _type != type; }
            operator bool() const { return _type != TokenType::EoF; }
            operator TokenType() const { return _type; }
        };

        class TokenIterator {
            StringIterator _current;
            StringIterator _end;

            Token _token;
            SourcePoint _point;
            Char _lookaheadChar;

        public:
            typedef std::input_iterator_tag iterator_category;

            TokenIterator(StringIterator begin, StringIterator end);

            TokenIterator& operator++();
            TokenIterator operator++(int);

            bool operator == (const TokenIterator& other);
            bool operator != (const TokenIterator& other);

            const Token& operator*() const { return _token; }
            const Token* operator->() const { return &_token; }
        private:
            Char consumeChar();
            void consumeIdentifier();
            void consumeSingleCharToken(const Char& c, const TokenType& type);
            void consumeWhitespace();
            void consumeString();
            void consumeDoubleQuotesString();
            void consumeSingleQuotesString();
            void consumeNumber();
            void consumeOperator();
            void consumeNext();

            bool isEoF() { return _lookaheadChar == 0; }
            bool isChar() { return (_lookaheadChar >= 'a' && _lookaheadChar <= 'z') || (_lookaheadChar >= 'A' && _lookaheadChar <= 'Z') || (_lookaheadChar > 127); }
            bool isNumber() { return _lookaheadChar >= '0' && _lookaheadChar <= '9'; }        
            bool isWhitespace() { return _lookaheadChar == ' ' || _lookaheadChar == '\t' || _lookaheadChar == '\n' || _lookaheadChar == '\r'; }
            bool isIdentifierLeadChar() { return isChar() || _lookaheadChar == '_'; }
            bool isOperatorLeadChar() {
                std::string op;
                op += _lookaheadChar;
                return operatorSymbols.find(op) != operatorSymbols.end();
            }
            bool isIdentifierTailChar() { return isIdentifierLeadChar() || isNumber(); }
            bool isDoubleQuoteChar() { return _lookaheadChar == '"'; }
            bool isSingleQuoteChar() { return _lookaheadChar == '\''; }
        };

        TokenIterator begin();
        TokenIterator end();
    };
};

std::ostream& operator << (std::ostream& out, rvm::Lexer::Token& token);

#endif
