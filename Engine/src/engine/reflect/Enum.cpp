#include "Enum.h"

#include "engine/asset/binary/Object.h"
#include "engine/asset/binary/Type.h"

namespace se::reflect
{
    Enum::Enum(void(* init)(Enum*)): Type{nullptr, 0, asset::binary::Type::Int32 }
    {
        init(this);
    }

    Enum::Enum(const char*, size_t, const std::initializer_list<EnumValue>& init): Type{ nullptr, 0, asset::binary::Type::Int32 }
    , values{ init }
    {
    }

    const std::string& Enum::ToString(int value) const
    {
        SPARK_ASSERT(value < static_cast<int>(values.size()));
        return values[value].name;
    }

    int Enum::FromString(const std::string& str) const
    {
        auto it = std::ranges::find_if(values, [str](const EnumValue& value) { return value.name == str; });
        if (it != values.end())
        {
            return it->value;
        }

        SPARK_ASSERT(false);
        return -1;
    }

    void Enum::Serialize(const void* obj, asset::binary::Object& parentObj, const std::string& fieldName) const
    {
        if (!fieldName.empty())
        {
            parentObj.Set(fieldName, ToString(*(int*)obj));
        }
        else
        {
            parentObj.Set("val", ToString(*(int*)obj));
        }
    }

    void Enum::Deserialize(void* obj, asset::binary::Object& parentObj, const std::string& fieldName) const
    {
        if (!fieldName.empty())
        {
            *(int*)obj = FromString(parentObj.Get<std::string>(fieldName));
        }
        else
        {
            *(int*)obj = FromString(parentObj.Get<std::string>("val"));
        }
    }

    asset::binary::StructLayout Enum::GetStructLayout(const void*) const
    {
        asset::binary::StructLayout structLayout = {{ asset::binary::CreateFixedString32("val"), asset::binary::Type::String }};
        return structLayout;
    }

    asset::binary::Type Enum::GetBinaryType() const
    {
        return asset::binary::Type::String;
    }
}
