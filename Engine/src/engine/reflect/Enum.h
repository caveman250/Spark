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

        Enum(void (*init)(Enum*));
        Enum(const char*, size_t, const std::initializer_list<EnumValue>& init);

        const std::string& ToString(int value) const;
        int FromString(const std::string& str) const;

        void Serialize(const void* obj, asset::binary::Object& parentObj, const std::string& fieldName) const override;
        void Deserialize(void* obj, asset::binary::Object& parentObj, const std::string& fieldName) const override;
        asset::binary::StructLayout GetStructLayout(const void*) const override;
        asset::binary::Type GetBinaryType() const override;
    };
};
