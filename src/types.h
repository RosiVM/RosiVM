#ifndef RVM_TYPES_H
#define RVM_TYPES_H

namespace rvm {
    namespace type {

        class Type {
        public:
            virtual bool isInvocable() { return false; }
            // virtual rvm::ast::ModuleMember* lookupOverload(std::vector<ptr_value>& args) { return nullptr; }
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

        PrimitiveType* getInt();
        PrimitiveType* getFloat();
        PrimitiveType* getBool();
        PrimitiveType* getString();
    };
};

#endif