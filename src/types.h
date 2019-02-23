#ifndef RVM_TYPES_H
#define RVM_TYPES_H

#include <vector>

namespace rvm {
    namespace type {
        
        class FunctionSignature;

        class Type {
            std::vector<FunctionSignature*> _functionSignatureTypes;
        public:
            Type() : _functionSignatureTypes() {}
            virtual std::vector<FunctionSignature*>& functionSignatureTypes() { return _functionSignatureTypes; }
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

        class SignatureType : public Type {
            Type* _returnType;
            std::vector<Type*> _argumentTypes;
        public:
            SignatureType(Type* returnType, std::vector<Type*> argumentTypes) : _returnType(returnType), _argumentTypes(std::move(argumentTypes)) {}
            Type* returnType() { return _returnType; }
            std::vector<Type*> argumentTypes() { return _argumentTypes; }
        };

        PrimitiveType* getInt();
        PrimitiveType* getFloat();
        PrimitiveType* getBool();
        PrimitiveType* getString();
    };
};

#endif