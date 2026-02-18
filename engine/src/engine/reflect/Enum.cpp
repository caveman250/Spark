#include "Enum.h"

#include "engine/asset/binary/Object.h"
#include "engine/asset/binary/Type.h"

namespace se::reflect
{
    Enum::Enum(): Type{"", 0, asset::binary::Type::String }
    {
    }

    Enum::Enum(const char*, size_t, const std::initializer_list<EnumValue>& init): Type{ "", 0, asset::binary::Type::String }
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
        const auto it = std::ranges::find_if(values, [str](const EnumValue& value) { return value.name == str; });
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
            parentObj.Set(fieldName, ToString(*static_cast<const int*>(obj)));
        }
        else
        {
            parentObj.Set("val", ToString(*static_cast<const int*>(obj)));
        }
    }

    void Enum::Deserialize(void* obj, asset::binary::Object& parentObj, const std::string& fieldName) const
    {
        if (!fieldName.empty())
        {
            *static_cast<int*>(obj) = FromString(parentObj.Get<std::string>(fieldName));
        }
        else
        {
            *static_cast<int*>(obj) = FromString(parentObj.Get<std::string>("val"));
        }
    }

    asset::binary::StructLayout Enum::GetStructLayout(const void*) const
    {
        asset::binary::StructLayout structLayout = {{ asset::binary::CreateFixedString64("val"), asset::binary::Type::String }};
        return structLayout;
    }

    asset::binary::Type Enum::GetBinaryType() const
    {
        return asset::binary::Type::String;
    }
}
