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
            parentObj.Set(fieldName, *(int*)obj);
        }
        else
        {
            parentObj.Set("val", *(int*)obj);
        }
    }

    void Enum::Deserialize(void* obj, asset::binary::Object& parentObj, const std::string& fieldName) const
    {
        if (!fieldName.empty())
        {
            *(int*)obj = parentObj.Get<int>(fieldName);
        }
        else
        {
            *(int*)obj = parentObj.Get<int>("val");
        }
    }

    asset::binary::StructLayout Enum::GetStructLayout() const
    {
        asset::binary::StructLayout structLayout = {{ asset::binary::CreateFixedString32("val"), asset::binary::Type::Int32 }};
        return structLayout;
    }
}
