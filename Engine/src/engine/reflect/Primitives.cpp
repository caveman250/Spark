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

    struct Type_UInt32 : Type
    {
        Type_UInt32() : Type{"uint32", sizeof(uint32_t), asset::binary::Type::Uint32 }
        {
        }
    };

    template <>
    Type* getPrimitiveDescriptor<uint32_t>()
    {
        static Type_UInt32 typeDesc;
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

    struct Type_VoidPtr : Type
    {
        Type_VoidPtr() : Type{"void*", sizeof(void*), asset::binary::Type::Invalid }
        {
        }
    };

    template <>
    Type* getPrimitiveDescriptor<void*>()
    {
        static Type_VoidPtr typeDesc;
        return &typeDesc;
    }
}
