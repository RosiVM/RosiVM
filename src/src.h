#ifndef RVM_SRC_H
#define RVM_SRC_H

#include <string>
#include <map>
#include <exception>
#include <iostream>

namespace rvm {
    typedef typename std::string::iterator StringIterator;
    typedef typename std::string::value_type Char;

    struct SourcePoint {
        unsigned int line, column;
        SourcePoint() : line(1), column(1) {}

        std::string toString() const {
            return std::to_string(line) + std::string(":") + std::to_string(column);
        }

        bool operator==(const SourcePoint& other) const {
            return line == other.line && column == other.column;
        }
    };

    struct SourceSpan {
        SourcePoint start;
        SourcePoint end;

        std::string toString() const {
            return start == end ? start.toString() : start.toString() + std::string("-") + end.toString();
        }

        bool operator==(const SourceSpan& other) const {
            return start == other.start && end == other.end;
        }
    };

    struct StringSpan {
        StringIterator start;
        StringIterator end;

        std::string toString() const { return std::string(start, end); }
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

    class CompilerError : public std::exception {
        ErrorCode _code;
        SourceSpan _span;

        std::string _message;

    public:
        CompilerError(ErrorCode code, SourceSpan span);
        CompilerError(ErrorCode code, SourcePoint point);

        SourceSpan span() { return _span; }
        virtual const char* what() const throw() { return _message.c_str(); }
    };
};

std::ostream& operator<< (std::ostream& out, const rvm::SourcePoint& point);
std::ostream& operator<< (std::ostream& out, const rvm::SourceSpan& span);

#endif