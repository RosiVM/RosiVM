#include "parser.h"

using namespace std;
using namespace rvm;
using namespace rvm::ast;

typedef std::unique_ptr<rvm::ast::Statement> ptr_statement;
typedef std::unique_ptr<rvm::ast::TypeExpression> ptr_type;
typedef std::unique_ptr<rvm::ast::ValueExpression> ptr_value;

inline rvm::Lexer::Token rvm::Parser::consumeToken() {
    Token token = _lookaheadToken;
    if (!is<TokenType::EoF>()) {
        _lookaheadToken = *++_current;
        // Ignore whitespace.
        // TODO: Capture last comment before members for docs.
        while(is<TokenType::Whitespace>()) _lookaheadToken = *++_current;
    }

    return token;
}

ptr_value rvm::Parser::parseValueExpression() {
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
            auto token = consume<TokenType::Assignment>();
            auto rhs = parseValueExpression();
            lhs = std::make_unique<AssignmentExpression>(move(lhs), token, move(rhs));
        } else if (is<TokenType::AdditionAssignment>()) {
            // <AddAssign> ::= <Prec12Exp> add-assign <Expression>
            auto token = consume<TokenType::AdditionAssignment>();
            auto rhs = parseValueExpression();
            lhs = std::make_unique<AdditionAssignmentExpression>(move(lhs), token, move(rhs));
        } else if (is<TokenType::SubtractionAssignment>()) {
            // <SubtractAssign> ::= <Prec12Exp> subtract-assign <Expression>
            auto token = consume<TokenType::SubtractionAssignment>();
            auto rhs = parseValueExpression();
            lhs = std::make_unique<SubtractionAssignmentExpression>(move(lhs), token, move(rhs));
        } else if (is<TokenType::MultiplicationAssignment>()) {
            // <MultiplyAssign> ::= <Prec12Exp> multiply-assign <Expression>
            auto token = consume<TokenType::MultiplicationAssignment>();
            auto rhs = parseValueExpression();
            lhs = std::make_unique<MultiplicationAssignmentExpression>(move(lhs), token, move(rhs));
        } else if (is<TokenType::DivisionAssignment>()) {
            // <DivideAssign> ::= <Prec12Exp> divide-assign <Expression>
            auto token = consume<TokenType::DivisionAssignment>();
            auto rhs = parseValueExpression();
            lhs = std::make_unique<DivisionAssignmentExpression>(move(lhs), token, move(rhs));
        } else if (is<TokenType::BitAndAssignment>()) {
            // <BitAndAssign> ::= <Prec12Exp> bit-and-assign <Expression>
            auto token = consume<TokenType::BitAndAssignment>();
            auto rhs = parseValueExpression();
            lhs = std::make_unique<BitAndAssignmentExpression>(move(lhs), token, move(rhs));
        } else if (is<TokenType::BitXOrAssignment>()) {
            // <BitXorAssign> ::= <Prec12Exp> bit-xor-assign <Expression>
            auto token = consume<TokenType::BitXOrAssignment>();
            auto rhs = parseValueExpression();
            lhs = std::make_unique<BitXOrAssignmentExpression>(move(lhs), token, move(rhs));
        } else if (is<TokenType::BitOrAssignment>()) {
            // <BitOrAssign> ::= <Prec12Exp> bit-or-assign <Expression>
            auto token = consume<TokenType::BitOrAssignment>();
            auto rhs = parseValueExpression();
            lhs = std::make_unique<BitOrAssignmentExpression>(move(lhs), token, move(rhs));
        } else if (is<TokenType::ReminderAssignment>()) {
            // <ReminderAssign> ::= <Prec12Exp> reminder-assign <Expression>
            auto token = consume<TokenType::ReminderAssignment>();
            auto rhs = parseValueExpression();
            lhs = std::make_unique<ReminderAssignmentExpression>(move(lhs), token, move(rhs));
        } else if (is<TokenType::LeftShiftAssignment>()) {
            // <ShiftLeftAssign> ::= <Prec12Exp> shift-left-assign <Expression>
            auto token = consume<TokenType::LeftShiftAssignment>();
            auto rhs = parseValueExpression();
            lhs = std::make_unique<LeftShiftAssignmentExpression>(move(lhs), token, move(rhs));
        } else if (is<TokenType::RightShiftAssignment>()) {
            // <ShiftRightAssign> ::= <Prec12Exp> shift-right-assign <Expression>
            auto token = consume<TokenType::RightShiftAssignment>();
            auto rhs = parseValueExpression();
            lhs = std::make_unique<RightShiftAssignmentExpression>(move(lhs), token, move(rhs));
        }
    }
    return lhs;
}

ptr_value rvm::Parser::parsePrec1ValueExpression() {
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

ptr_value rvm::Parser::parsePrec2ValueExpression() {
    // ! Conditional Operators
    // <Prec2Exp> ::= <Prec3Exp> | <ConditionalOr>
    // <ConditionalOr> ::= <Prec2Exp> conditional-or <Prec3Exp>       
    ptr_value lhs = parsePrec3ValueExpression();
    while(is<TokenType::ConditionalOr>()) {
        auto token = consume<TokenType::ConditionalOr>();
        auto rhs = parsePrec3ValueExpression();
        lhs = std::make_unique<ConditionalOrExpression>(move(lhs), token, move(rhs));
    }
    return lhs;
}

ptr_value rvm::Parser::parsePrec3ValueExpression() {
    // ! Conditional Operators
    // <Prec3Exp> ::= <Prec4Exp> | <ConditionalAnd>
    // <ConditionalAnd> ::=  <Prec3Exp> conditional-and <Prec4Exp>
    ptr_value lhs = parsePrec4ValueExpression();
    while(is<TokenType::ConditionalAnd>()) {
        auto token = consume<TokenType::ConditionalAnd>();
        auto rhs = parsePrec4ValueExpression();
        lhs = std::make_unique<ConditionalAndExpression>(move(lhs), token, move(rhs));
    }
    return lhs;
}

ptr_value rvm::Parser::parsePrec4ValueExpression() {
    // ! Bitwise Operators
    // <Prec4Exp> ::= <Prec5Exp> | <BitwiseOr>
    // <BitwiseOr> ::= <Prec4Exp> bitwise-or <Prec5Exp>
    ptr_value lhs = parsePrec5ValueExpression();
    while(is<TokenType::BitwiseOr>()) {
        auto token = consume<TokenType::BitwiseOr>();
        auto rhs = parsePrec5ValueExpression();
        lhs = std::make_unique<BitwiseOrExpression>(move(lhs), token, move(rhs));
    }
    return lhs;
}

ptr_value rvm::Parser::parsePrec5ValueExpression() {
    // <Prec5Exp> ::= <Prec6Exp> | <BitwiseXOr>
    // <BitwiseXOr> ::= <Prec5Exp> bitwise-xor <Prec6Exp>
    ptr_value lhs = parsePrec6ValueExpression();
    while(is<TokenType::BitwiseXOr>()) {
        auto token = consume<TokenType::BitwiseXOr>();
        auto rhs = parsePrec6ValueExpression();
        lhs = std::make_unique<BitwiseXOrExpression>(move(lhs), token, move(rhs));
    }
    return lhs;
}

ptr_value rvm::Parser::parsePrec6ValueExpression() {
    // <Prec6Exp> ::= <Prec7Exp> | <BitwiseAnd>
    // <BitwiseAnd> ::= <Prec6Exp> bitwise-and <Prec7Exp>
    ptr_value lhs = parsePrec7ValueExpression();
    while(is<TokenType::BitwiseAnd>()) {
        auto token = consume<TokenType::BitwiseAnd>();
        auto rhs = parsePrec7ValueExpression();
        lhs = std::make_unique<BitwiseAndExpression>(move(lhs), token, move(rhs));
    }
    return lhs;
}

ptr_value rvm::Parser::parsePrec7ValueExpression() {
    // ! Equality Operators
    // <Prec7Exp> ::= <Prec8Exp> | <Equal> | <NotEqual>
    ptr_value lhs = parsePrec8ValueExpression();
    do {
        if (is<TokenType::Equal>()) {
            // <Equal> ::= <Prec7Exp> equal <Prec8Exp>
            auto token = consume<TokenType::Equal>();
            auto rhs = parsePrec8ValueExpression();
            lhs = std::make_unique<EqualExpression>(move(lhs), token, move(rhs));
        } else if (is<TokenType::NotEqual>()) {
            // <NotEqual> ::= <Prec7Exp> not-equal <Prec8Exp>
            auto token = consume<TokenType::NotEqual>();
            auto rhs = parsePrec8ValueExpression();
            lhs = std::make_unique<NotEqualExpression>(move(lhs), token, move(rhs));
        } else break;
    } while(true);
    return lhs;
}

ptr_value rvm::Parser::parsePrec8ValueExpression() {
    // ! Comparison Operators
    // <Prec8Exp> ::= <Prec9Exp> | <LessThan> | <GreaterThan> | <LessOrEqual> | <GreaterOrEqual>
    ptr_value lhs = parsePrec9ValueExpression();
    do {
        if (is<TokenType::Less>()) {
            // <LessThan> ::= <Prec8Exp> less-than <Prec9Exp>
            auto token = consume<TokenType::Less>();
            auto rhs = parsePrec9ValueExpression();
            lhs = std::make_unique<LessThanExpression>(move(lhs), token, move(rhs));
        } else if (is<TokenType::Greater>()) {
            // <GreaterThan> ::= <Prec8Exp> greater-than <Prec9Exp>
            auto token = consume<TokenType::Greater>();
            auto rhs = parsePrec9ValueExpression();
            lhs = std::make_unique<GreaterThanExpression>(move(lhs), token, move(rhs));
        } else if (is<TokenType::LessOrEqual>()) {
            // <LessOrEqual> ::= <Prec8Exp> less-or-equal <Prec9Exp>
            auto token = consume<TokenType::LessOrEqual>();
            auto rhs = parsePrec9ValueExpression();
            lhs = std::make_unique<LessOrEqualExpression>(move(lhs), token, move(rhs));
        } else if (is<TokenType::GreaterOrEqual>()) {
            // <GreaterOrEqual> ::= <Prec8Exp> greater-or-equal <Prec9Exp>
            auto token = consume<TokenType::GreaterOrEqual>();
            auto rhs = parsePrec9ValueExpression();
            lhs = std::make_unique<GreaterOrEqualExpression>(move(lhs), token, move(rhs));
        } else break;
    } while(true);
    return lhs;
}

ptr_value rvm::Parser::parsePrec9ValueExpression() {
    // ! Shift Operators
    // <Prec9Exp> ::= <Prec10Exp> | <LeftShift> | <RightShift>
    ptr_value lhs = parsePrec10ValueExpression();

    do {
        if (is<TokenType::LeftShift>()) {
            // <LeftShift> ::= <Prec9Exp> shift-left <Prec10Exp>
            auto token = consume<TokenType::LeftShift>();
            auto rhs = parsePrec10ValueExpression();
            lhs = std::make_unique<LeftShiftExpression>(move(lhs), token, move(rhs));
        } else if (is<TokenType::RightShift>()) {
            // <RightShift> ::= <Prec9Exp> shift-right <Prec10Exp>
            auto token = consume<TokenType::RightShift>();
            auto rhs = parsePrec10ValueExpression(); 
            lhs = std::make_unique<RightShiftExpression>(move(lhs), token, move(rhs));
        } else break;
    } while(true);
    return lhs;
}

ptr_value rvm::Parser::parsePrec10ValueExpression() {
    // Precedence 10 operators: Cumulative Operators
    // <Prec10Exp> ::= <Prec11Exp> | <Add> | <Subtract>

    // <Prec11Exp>
    ptr_value prec10Exp = parsePrec11ValueExpression();
    do {
        if (is<TokenType::Plus>()) {
            // <Add> ::= <Prec10Exp> plus <Prec11Exp>
            auto token = consume<TokenType::Plus>();
            auto rhs = parsePrec11ValueExpression();
            prec10Exp = std::make_unique<AddExpression>(move(prec10Exp), token, move(rhs));
        } else if (is<TokenType::Minus>()) {
            // <Subtract> ::= <Prec10Exp> minus <Prec11Exp>
            auto token = consume<TokenType::Minus>();
            auto rhs = parsePrec11ValueExpression();
            prec10Exp = std::make_unique<SubtractExpression>(move(prec10Exp), token, move(rhs));
        } else break;
    } while(true);
    return prec10Exp;
}

ptr_value rvm::Parser::parsePrec11ValueExpression() {
    // Precedence 11 operators: Multiplicative Operators
    // <Prec11Exp> ::= <Prec12Exp> | <Multiply> | <Divide> | <Reminder>

    // <Prec12Exp>
    ptr_value prec11Exp = parsePrec12ValueExpression();
    do {
        if (is<TokenType::Multiply>()) {
            // <Multiply> ::= <Prec11Exp> multiply <Prec12Exp>
            auto token = consume<TokenType::Multiply>();
            auto rhs = parsePrec12ValueExpression();
            prec11Exp = std::make_unique<MultiplyExpression>(move(prec11Exp), token, move(rhs));
        } else if (is<TokenType::Divide>()) {
            // <Divide> ::= <Prec11Exp> divide <Prec12Exp>
            auto token = consume<TokenType::Divide>();
            auto rhs = parsePrec12ValueExpression();
            prec11Exp = std::make_unique<DivideExpression>(move(prec11Exp), token, move(rhs));
        } else if (is<TokenType::Reminder>()) {
            // <Reminder> ::= <Prec11Exp> reminder <Prec12Exp>
            auto token = consume<TokenType::Reminder>();
            auto rhs = parsePrec12ValueExpression();
            prec11Exp = std::make_unique<ReminderExpression>(move(prec11Exp), token, move(rhs));
        } else break;
    } while(true);
    return prec11Exp;
}

ptr_value rvm::Parser::parsePrec12ValueExpression() {
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
        auto token = consume<TokenType::ConditionalNot>();
        auto expression = parsePrec12ValueExpression();
        return std::make_unique<ConditionalNotExpression>(token, move(expression));
    } else if (is<TokenType::Plus>()) {
        auto token = consume<TokenType::Plus>();
        auto expression = parsePrec12ValueExpression();
        return std::make_unique<UnaryPlusExpression>(token, move(expression));
    } else if (is<TokenType::Minus>()) {
        auto token = consume<TokenType::Minus>();
        auto expression = parsePrec12ValueExpression();
        return std::make_unique<UnaryMinusExpression>(token, move(expression));
    } else if (is<TokenType::Increment>()) {
        auto token = consume<TokenType::Increment>();
        auto expression = parsePrec12ValueExpression();
        return std::make_unique<PreIncrementExpression>(token, move(expression));
    } else if (is<TokenType::Decrement>()) {
        auto token = consume<TokenType::Decrement>();
        auto expression = parsePrec12ValueExpression();
        return std::make_unique<PreDecrementExpression>(token, move(expression));
    } else if (is<TokenType::BitComplement>()) {
        auto token = consume<TokenType::BitComplement>();
        auto expression = parsePrec12ValueExpression();
        return std::make_unique<BitComplementExpression>(token, move(expression));
    } else return parsePrec13ValueExpression();
}

ptr_value rvm::Parser::parsePrec13ValueExpression() {
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
            auto token = consume<TokenType::Increment>();
            prec13Exp = std::make_unique<PostIncrementExpression>(move(prec13Exp), token);
        } else if (is<TokenType::Decrement>()) {
            // <PostDecrement> ::= <Prec13Exp> decrement
            auto token = consume<TokenType::Decrement>();
            prec13Exp = std::make_unique<PostDecrementExpression>(move(prec13Exp), token);
        } else break;
    } while(true);
    return prec13Exp;
}

unique_ptr<ConstStatement> rvm::Parser::parseConstStatement() {
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

unique_ptr<ReturnStatement> rvm::Parser::parseReturnStatement() {
    consume<TokenType::ReturnKeyword>();
    ptr_value value = nullptr;
    if (!is<TokenType::Semicolon>()) value = parseValueExpression();
    consume<TokenType::Semicolon>();
    return std::make_unique<ReturnStatement>(move(value));
}

ptr_statement rvm::Parser::parseStatement() {
    if (is<TokenType::ConstKeyword>()) return parseConstStatement();
    if (is<TokenType::ReturnKeyword>()) return parseReturnStatement();

    // TODO: if, do, while, for, throw, catch etc.

    ptr_statement expression = parseValueExpression();
    consume<TokenType::Semicolon>();
    return expression;
}

unique_ptr<CodeBlock> rvm::Parser::parseCodeBlock() {
    consume<TokenType::LeftBrace>();
    vector<ptr_statement> _statements;
    while(!is<TokenType::RightBrace>()) {
        if (is<TokenType::EoF>()) throw CompilerError(UnexpectedParserEoF, span());
        _statements.push_back(parseStatement());
    }
    consume<TokenType::RightBrace>();
    return std::make_unique<CodeBlock>(move(_statements));
}

ptr_type rvm::Parser::parseTypeExpression() {
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

unique_ptr<FunctionArgument> rvm::Parser::consumeFunctionArgument() {
    Token identifier;
    if (is<TokenType::Identifier>()) {
        identifier = consume<TokenType::Identifier>();
    }
    consume<TokenType::Colon>();
    ptr_type type = parseTypeExpression();

    return std::make_unique<FunctionArgument>(identifier, move(type));
}

unique_ptr<FunctionPrototype> rvm::Parser::consumeFunctionPrototype() {
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

unique_ptr<Function> rvm::Parser::consumeFunction() {
    auto functionKeywordToken = consume<TokenType::FunctionKeyword>();
    auto identifier = consume<TokenType::Identifier>();
    auto proto = consumeFunctionPrototype();
    auto block = parseCodeBlock();

    return std::make_unique<Function>(identifier, move(proto), move(block));
}

unique_ptr<FunctionDeclaration> rvm::Parser::consumeFunctionDeclaration(Token& declareKeyword) {
    auto functionKeywordToken = consume<TokenType::FunctionKeyword>();
    auto identifier = consume<TokenType::Identifier>();
    auto proto = consumeFunctionPrototype();
    auto semicolon = consume<TokenType::Semicolon>();

    return std::make_unique<FunctionDeclaration>(identifier, move(proto));
}

void rvm::Parser::parseModuleMembers() {
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
