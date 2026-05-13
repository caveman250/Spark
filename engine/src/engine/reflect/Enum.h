#pragma once
#include "Type.h"

namespace se::reflect
{
    template <typename T>
    struct EnumValue
    {
        std::string name;
        T value;
    };

    struct EnumBase : Type
    {
        EnumBase()
            : Type{"", 0, asset::binary::Type::String }
        {}

        virtual std::vector<const std::string*> GetValueNames() const = 0;
        virtual size_t GetIndexForValue(void* obj) const = 0;
        virtual void SetValueFromIndex(size_t index, void* obj) const = 0;
    };

    template <typename T>
    struct Enum : EnumBase
    {
        std::vector<EnumValue<T>> values;

        Enum();
        Enum(const char*, size_t, const std::initializer_list<EnumValue<T>>& init);

        const std::string& ToString(T value) const;
        T FromString(const std::string& str) const;

        bool IsEnum() const override { return true; }

        void Serialize(const void* obj, asset::binary::Object& parentObj, const std::string& fieldName) const override;
        void Deserialize(void* obj, asset::binary::Object& parentObj, const std::string& fieldName) const override;
        asset::binary::StructLayout GetStructLayout(const void*) const override;
        asset::binary::Type GetBinaryType() const override;

        std::vector<const std::string*> GetValueNames() const override;
        size_t GetIndexForValue(void* obj) const override;
        void SetValueFromIndex(size_t index, void* obj) const override;
    };

    template <typename T>
    Enum<T>::Enum()
    {
    }

    template <typename T>
    Enum<T>::Enum(const char*, size_t, const std::initializer_list<EnumValue<T>>& init)
        : values{ init }
    {
    }

    template <typename T>
    const std::string& Enum<T>::ToString(T value) const
    {
        const auto it = std::ranges::find_if(values,
            [value](const EnumValue<T>& it)
            {
                return it.value == value;
            });

        if (it != values.end())
        {
            return it->name;
        }

        SPARK_ASSERT(false);
        static constexpr std::string empty = {};
        return empty;
    }

    template <typename T>
    T Enum<T>::FromString(const std::string& str) const
    {
        const auto it = std::ranges::find_if(values, [str](const EnumValue<T>& value) { return value.name == str; });
        if (it != values.end())
        {
            return it->value;
        }

        SPARK_ASSERT(false);
        return T();
    }

    template <typename T>
    void Enum<T>::Serialize(const void* obj, asset::binary::Object& parentObj, const std::string& fieldName) const
    {
        if (!fieldName.empty())
        {
            parentObj.Set(fieldName, ToString(*static_cast<const T*>(obj)));
        }
        else
        {
            parentObj.Set("val", ToString(*static_cast<const T*>(obj)));
        }
    }

    template <typename T>
    void Enum<T>::Deserialize(void* obj, asset::binary::Object& parentObj, const std::string& fieldName) const
    {
        if (!fieldName.empty())
        {
            *static_cast<T*>(obj) = FromString(parentObj.Get<std::string>(fieldName));
        }
        else
        {
            *static_cast<T*>(obj) = FromString(parentObj.Get<std::string>("val"));
        }
    }

    template <typename T>
    asset::binary::StructLayout Enum<T>::GetStructLayout(const void*) const
    {
        asset::binary::StructLayout structLayout = {{ asset::binary::CreateFixedString64("val"), asset::binary::Type::String }};
        return structLayout;
    }

    template <typename T>
    asset::binary::Type Enum<T>::GetBinaryType() const
    {
        return asset::binary::Type::String;
    }

    template<typename T>
    std::vector<const std::string*> Enum<T>::GetValueNames() const
    {
        std::vector<const std::string*> ret;
        for (const auto& value : values)
        {
            ret.push_back(&value.name);
        }
        return ret;
    }

    template<typename T>
    size_t Enum<T>::GetIndexForValue(void* obj) const
    {
        const T& objTyped = *static_cast<T*>(obj);
        const auto it = std::ranges::find_if(values, [objTyped](const EnumValue<T>& value) { return value.value == objTyped; });
        return std::distance(values.begin(), it);
    }

    template<typename T>
    void Enum<T>::SetValueFromIndex(size_t index,
        void* obj) const
    {
        *static_cast<T*>(obj) = values[index].value;
    }
};
