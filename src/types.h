#ifndef RVM_TYPES_H
#define RVM_TYPES_H

#include <vector>

namespace rvm {
    namespace type {

        class SignatureType;

        class Type {
            std::vector<SignatureType*> _callSignatures;
        public:
            Type() : _callSignatures() {}
            virtual std::vector<SignatureType*>& callSignatures() { return _callSignatures; }
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

        /// A function signature type.
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