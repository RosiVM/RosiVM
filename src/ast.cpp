#include "ast.h"

using namespace std;

const string rvm::ast::UnaryOperatorString[8] {
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

const string rvm::ast::BinaryOperatorString[29] {
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
