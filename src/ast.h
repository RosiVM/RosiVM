#ifndef RVM_AST_H
#define RVM_AST_H

#include <string>
#include <vector>
#include "lexer.h"

namespace rvm {
    namespace ast {
        typedef typename rvm::Lexer::Token Token;

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
        class IdentifierExpression;
        class ConstantValueExpression;
        class MemberAccessExpression;
        class InvocationExpression;
        class ConditionalIfExpression;

        class UnaryExpression;
        class BinaryExpression;

        class TypeExpression;
        class PrimitiveTypeExpression;

        typedef std::unique_ptr<Statement> ptr_statement;
        typedef std::unique_ptr<TypeExpression> ptr_type;
        typedef std::unique_ptr<ValueExpression> ptr_value;

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
        extern const std::string UnaryOperatorString[8];

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
        extern const std::string BinaryOperatorString[29];

        inline const std::string toString(UnaryOperator op) { return UnaryOperatorString[op]; }
        inline const std::string toString(BinaryOperator op) { return BinaryOperatorString[op]; }

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

        class FunctionPrototype {
            std::vector<std::unique_ptr<FunctionArgument> > _args;
            ptr_type _returnType;
        public:
            FunctionPrototype(std::vector<std::unique_ptr<FunctionArgument> > args, ptr_type returnType) :
                _args(std::move(args)),
                _returnType(std::move(returnType)) {}
            
            std::vector<std::unique_ptr<FunctionArgument> >& args() { return _args; }
            ptr_type& returnType() { return _returnType; }
        };

        class Function : public ModuleMember {
            Token _identifier;
            std::unique_ptr<FunctionPrototype> _proto;
            std::unique_ptr<CodeBlock> _block;
        public:
            Function(Token identifier, std::unique_ptr<FunctionPrototype> proto, std::unique_ptr<CodeBlock> block) :
                _identifier(identifier),
                _proto(move(proto)),
                _block(move(block)) {}

            std::string name() { return _identifier.value<std::string>(); }
            std::vector<std::unique_ptr<FunctionArgument> >& args() { return _proto->args(); }
            ptr_type& returnType() { return _proto->returnType(); }
            std::unique_ptr<FunctionPrototype>& proto() { return _proto; }
            std::unique_ptr<CodeBlock>& codeBlock() { return _block; }

            void visit(ModuleMemberVisitor* visitor) override { visitor->on(this); }
        };

        class FunctionDeclaration : public ModuleMember {
            Token _identifier;
            std::unique_ptr<FunctionPrototype> _proto;
        public:
            FunctionDeclaration(Token identifier, std::unique_ptr<FunctionPrototype> proto) :
                _identifier(identifier),
                _proto(std::move(proto)) {}

            std::string name() { return _identifier.value<std::string>(); }
            std::vector<std::unique_ptr<FunctionArgument> >& args() { return _proto->args(); }
            ptr_type& returnType() { return _proto->returnType(); }
            std::unique_ptr<FunctionPrototype>& proto() { return _proto; }

            void visit(ModuleMemberVisitor* visitor) override { visitor->on(this); }
        };

        class FunctionArgument {
            Token _identifier;
            ptr_type _type;
        public:
            FunctionArgument(Token identifier, ptr_type type) : _identifier(identifier), _type(move(type)) {}

            std::string name() { return _identifier.value<std::string>(); }
            ptr_type& type() { return _type; }
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
            std::vector<ptr_statement> _statements;
        public:
            CodeBlock(std::vector<ptr_statement> statements) : _statements(std::move(statements)) {}
            std::vector<ptr_statement>& statements() { return _statements; }
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

            std::string name() { return _identifier.value<std::string>(); }
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
            std::string name() { return _identifier.value<std::string>(); }
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
            std::string name() { return _name.value<std::string>(); }
            ushort precedence() override { return 13; }
            void visit(StatementVisitor* visitor) override { visitor->on(this); }
        };
        class InvocationExpression : public ValueExpression {
            std::vector<ptr_value> _values;
            ptr_value _operand;
        public:
            InvocationExpression(ptr_value lhs, std::vector<ptr_value> values) : _operand(move(lhs)), _values(move(values)) {}
            ptr_value& operand() { return _operand; }
            ushort precedence() override { return 13; }
            std::vector<ptr_value>& values() { return _values; }
            void visit(StatementVisitor* visitor) override { visitor->on(this); }
        };

        UNARY_EXPRESSION_CLASS(PostIncrementExpression, PostIncrementOperator, 13);
        UNARY_EXPRESSION_CLASS(PostDecrementExpression, PostDecrementOperator, 13);
    };
};

#endif