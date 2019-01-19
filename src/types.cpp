#include "types.h"

using namespace rvm::type;

PrimitiveType primitiveInt = PrimitiveType(rvm::type::PrimitiveType::Int);
PrimitiveType primitiveFloat = PrimitiveType(rvm::type::PrimitiveType::Float);
PrimitiveType primitiveBool = PrimitiveType(rvm::type::PrimitiveType::Bool);
PrimitiveType primitiveString = PrimitiveType(rvm::type::PrimitiveType::String);

PrimitiveType* rvm::type::getInt() { return &primitiveInt; }
PrimitiveType* rvm::type::getFloat() { return &primitiveFloat; }
PrimitiveType* rvm::type::getBool() { return &primitiveBool; }
PrimitiveType* rvm::type::getString() { return &primitiveString; }
