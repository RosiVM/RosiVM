#include "source.h"

using namespace std;
using namespace rvm;

std::map<rvm::ErrorCode, std::string> errorCodeDescription {
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

rvm::CompilerError::CompilerError(ErrorCode code, SourceSpan span) :
    _code(code),
    _span(span),
    _message(errorCodeDescription[code] + " ("s + span.toString() + ")") {
}

rvm::CompilerError::CompilerError(ErrorCode code, SourcePoint point) : CompilerError(code, {point, point}) {}
