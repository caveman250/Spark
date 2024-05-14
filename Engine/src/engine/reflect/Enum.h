#pragma once
#include "Type.h"

namespace se::reflect
{
    struct EnumValue
    {
        std::string name;
        int value;
    };

    struct Enum : Type
    {
        std::vector<EnumValue> values;

        Enum(void (*init)(Enum *));
        Enum(const char *, size_t, const std::initializer_list<EnumValue> &init);

        const std::string& ToString(int value);
        int FromString(const std::string& str);
    };
}
