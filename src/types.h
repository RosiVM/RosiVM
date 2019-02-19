#ifndef RVM_TYPES_H
#define RVM_TYPES_H

namespace rvm {
    namespace type {

        class Type {
        public:
            virtual bool isInvocable() { return false; }
        };

        class PrimitiveType : public Type {
        public:
            enum Type {
                Int,
                Float,
                Bool,
                String,
            };
        private:
            Type _type;
        public:
            PrimitiveType(Type type) : _type(type) {}
            Type type() { return _type; }
        };

        class FunctionSignature : public Type {
            Type* _returnType;
        public:
            // TODO: Either use some sort of shared pointers, or normalize types on the go and make the TypeChecker keep their lifetime...
            FunctionSignature(Type* returnType) : _returnType(returnType) {}
            Type* returnType() { return _returnType; }
        };

        PrimitiveType* getInt();
        PrimitiveType* getFloat();
        PrimitiveType* getBool();
        PrimitiveType* getString();
    };
};

#endif