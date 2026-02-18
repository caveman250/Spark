#include "engine/asset/binary/Database.h"
#include "Reflect.h"

namespace se::reflect
{
#define DEFINE_PRIMITIVE(type, binaryType)\
struct Type_##type : Type {\
Type_##type() : Type{#type, sizeof(type), binaryType} {\
}\
void Serialize(const void* obj, asset::binary::Object& parentObj, const std::string& fieldName) const override\
{\
    if (!fieldName.empty())\
        parentObj.Set(fieldName, *(type*)obj);\
    else\
        parentObj.Set("val", *(type*)obj);\
}\
void Deserialize(void* obj, asset::binary::Object& parentObj, const std::string& fieldName) const override\
{\
if (!fieldName.empty())\
    *(type*)obj = parentObj.Get<type>(fieldName);\
else\
    *(type*)obj = parentObj.Get<type>("val");\
}\
asset::binary::StructLayout GetStructLayout(const void*) const override\
{\
    asset::binary::StructLayout structLayout = {{ asset::binary::CreateFixedString64("val"), binaryType }};\
    return structLayout;\
}\
};\
template<>\
Type* GetPrimitiveDescriptor<type>(){\
static Type_##type typeDesc;\
return &typeDesc;\
}

    DEFINE_PRIMITIVE(bool, asset::binary::Type::Bool)
    DEFINE_PRIMITIVE(char, asset::binary::Type::Int8)
    DEFINE_PRIMITIVE(int8_t, asset::binary::Type::Int8)
    DEFINE_PRIMITIVE(uint8_t, asset::binary::Type::Uint8)
    DEFINE_PRIMITIVE(int16_t, asset::binary::Type::Int16)
    DEFINE_PRIMITIVE(uint16_t, asset::binary::Type::Uint16)
    DEFINE_PRIMITIVE(int32_t, asset::binary::Type::Int32)
    DEFINE_PRIMITIVE(uint32_t, asset::binary::Type::Uint32)
    DEFINE_PRIMITIVE(int64_t, asset::binary::Type::Int64)
    DEFINE_PRIMITIVE(uint64_t, asset::binary::Type::Uint64)
#if SPARK_PLATFORM_MAC
    DEFINE_PRIMITIVE(size_t, asset::binary::Type::Uint64)
#endif
    DEFINE_PRIMITIVE(double, asset::binary::Type::Double)
    DEFINE_PRIMITIVE(float, asset::binary::Type::Float)
    using namespace std;
    DEFINE_PRIMITIVE(string, asset::binary::Type::String)

    struct Type_Void : Type
    {
        Type_Void() : Type{"void", 0, asset::binary::Type::Invalid }
        {

        }

        void Serialize(const void* , asset::binary::Object& , const std::string& ) const override
        {
            SPARK_ASSERT(false, "Unimplemented");
        }

        void Deserialize(void* , asset::binary::Object& , const std::string& ) const override
        {
            SPARK_ASSERT(false, "Unimplemented");
        }

        asset::binary::StructLayout GetStructLayout(const void*) const override
        {
            asset::binary::StructLayout structLayout = {{ asset::binary::CreateFixedString64("val"), asset::binary::Type::Invalid }};
            return structLayout;
        }
    };

    template <>
    Type* GetPrimitiveDescriptor<void>()
    {
        static Type_Void typeDesc;
        return &typeDesc;
    }

    struct Type_ObjectBase : Type
    {
        Type_ObjectBase() : Type{"ObjectBase", sizeof(ObjectBase), asset::binary::Type::Invalid }
        {

        }

        void Serialize(const void* , asset::binary::Object& , const std::string& ) const override
        {
            SPARK_ASSERT(false, "Unimplemented");
        }

        void Deserialize(void* , asset::binary::Object& , const std::string& ) const override
        {
            SPARK_ASSERT(false, "Unimplemented");
        }

        asset::binary::StructLayout GetStructLayout(const void*) const override
        {
            asset::binary::StructLayout structLayout = {{ asset::binary::CreateFixedString64("val"), asset::binary::Type::Invalid }};
            return structLayout;
        }
    };

    template <>
    Type* GetPrimitiveDescriptor<ObjectBase>()
    {
        static Type_ObjectBase typeDesc;
        return &typeDesc;
    }

    struct Type_Id : Type
    {
        Type_Id() : Type{"Id", sizeof(uint64_t), asset::binary::Type::Uint64 }
        {

        }

        void Serialize(const void* obj, asset::binary::Object& parentObj, const std::string& fieldName) const override
        {
            if (!fieldName.empty())
                parentObj.Set(fieldName, *static_cast<const uint64_t*>(obj));
            else
                parentObj.Set("val", *static_cast<const uint64_t*>(obj));
        }

        void Deserialize(void* obj, asset::binary::Object& parentObj, const std::string& fieldName) const override
        {
            if (!fieldName.empty())
                *static_cast<uint64_t*>(obj) = parentObj.Get<uint64_t>(fieldName);
            else
                *static_cast<uint64_t*>(obj) = parentObj.Get<uint64_t>("val");
        }

        asset::binary::StructLayout GetStructLayout(const void*) const override
        {
            asset::binary::StructLayout structLayout = {{ asset::binary::CreateFixedString64("val"), asset::binary::Type::Uint64 }};
            return structLayout;
        }
    };

    template <>
    Type* GetPrimitiveDescriptor<ecs::Id>()
    {
        static Type_Id typeDesc;
        return &typeDesc;
    }
}
