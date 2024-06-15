#include "Enum.h"

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

    const std::string& Enum::ToString(int value)
    {
        SPARK_ASSERT(value < static_cast<int>(values.size()));
        return values[value].name;
    }

    int Enum::FromString(const std::string& str)
    {
        auto it = std::ranges::find_if(values, [str](const EnumValue& value) { return value.name == str; });
        if (it != values.end())
        {
            return it->value;
        }

        SPARK_ASSERT(false);
        return -1;
    }
}
