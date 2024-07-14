#include <engine/asset/binary/Database.h>
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
asset::binary::StructLayout GetStructLayout() const override\
{\
    asset::binary::StructLayout structLayout = {{ asset::binary::CreateFixedString32("val"), binaryType }};\
    return structLayout;\
}\
};\
template<>\
Type* getPrimitiveDescriptor<type>(){\
static Type_##type typeDesc;\
return &typeDesc;\
}

    DEFINE_PRIMITIVE(int, asset::binary::Type::Int32)
    DEFINE_PRIMITIVE(uint32_t, asset::binary::Type::Uint32)
    DEFINE_PRIMITIVE(float, asset::binary::Type::Float)
    using namespace std;
    DEFINE_PRIMITIVE(string, asset::binary::Type::String)

    struct Type_VoidPtr : Type
    {
        Type_VoidPtr() : Type{"void*", sizeof(void*), asset::binary::Type::Invalid }
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

        asset::binary::StructLayout GetStructLayout() const override
        {
            asset::binary::StructLayout structLayout = {{ asset::binary::CreateFixedString32("val"), asset::binary::Type::Invalid }};
            return structLayout;
        }
    };

    template <>
    Type* getPrimitiveDescriptor<void*>()
    {
        static Type_VoidPtr typeDesc;
        return &typeDesc;
    }
}
