#include <engine/asset/binary/Database.h>
#include "Reflect.h"

namespace se::reflect
{
    struct Type_Int : Type
    {
        Type_Int() : Type{"int", sizeof(int), asset::binary::Type::Int32 }
        {
        }
    };

    template <>
    Type* getPrimitiveDescriptor<int>()
    {
        static Type_Int typeDesc;
        return &typeDesc;
    }

    struct Type_Float : Type
    {
        Type_Float() : Type{"float", sizeof(float), asset::binary::Type::Float}
        {
        }
    };

    template <>
    Type* getPrimitiveDescriptor<float>()
    {
        static Type_Float typeDesc;
        return &typeDesc;
    }

    struct Type_StdString : Type
    {
        Type_StdString() : Type{"std::string", sizeof(std::string), asset::binary::Type::String }
        {
        }
    };

    template <>
    Type* getPrimitiveDescriptor<std::string>()
    {
        static Type_StdString typeDesc;
        return &typeDesc;
    }
}