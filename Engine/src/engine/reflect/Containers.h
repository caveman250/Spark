#pragma once

#include "spark.h"
#include "TypeResolver.h"
#include "engine/asset/binary/Type.h"

namespace se::reflect
{
    struct Type_StdVector : Type
    {
        Type* itemType;

        size_t (*getSize)(const void*);
        const void* (*getItem)(const void*, size_t);
        std::function<void(void*&, size_t)> instantiate;
        std::function<void*(void*, size_t)> getRawItem;

        template <typename T>
        Type_StdVector(T*)
            : Type{"std::vector<>", sizeof(std::vector<T>), asset::binary::Type::Array},
              itemType{TypeResolver<T>::get()}
        {
            getSize = [](const void* vecPtr)
            {
                const auto& vec = *static_cast<const std::vector<T>*>(vecPtr);
                return vec.size();
            };

            getItem = [](const void* vecPtr, size_t index)
            {
                const auto& vec = *static_cast<const std::vector<T>*>(vecPtr);
                return static_cast<const void*>(&vec.at(index));
            };

            instantiate = [](void*& obj, size_t sz) -> void
            {
                auto& vec = *(std::vector<T>*)obj;
                vec.resize(sz);
            };

            getRawItem = [](void* vecPtr, size_t index) -> void* {
                auto& vec = *(std::vector<T>*)vecPtr;
                return &vec[index];
            };
        }

        std::string GetTypeName() const override;

        void Serialize(const void* obj, asset::binary::Object& parentObj, const std::string& fieldName) const override;
        void Deserialize(void* obj, asset::binary::Object& parentObj, const std::string& fieldName) const override;
        asset::binary::StructLayout GetStructLayout() const override;
    };

    template <typename T>
    class TypeResolver<std::vector<T>>
    {
    public:
        static Type* get()
        {
            static Type_StdVector typeDesc{(T*)nullptr};
            return &typeDesc;
        }
    };

    void SerializeArray(const void* obj, asset::binary::Object& parentObj, const std::string& fieldName, Type* itemType,
                        size_t (*getSize)(const void*), const void* (*getItem)(const void*, size_t));
    void DeserializeArray(void* obj, asset::binary::Object& parentObj, const std::string& fieldName, Type* itemType,
                          std::function<void(void*&)> instantiate, std::function<void*(void*, size_t)> getRawItem);

    template <size_t Size>
    struct Type_StdArray : Type
    {
        Type* itemType;
        size_t (*getSize)(const void*);
        const void* (*getItem)(const void*, size_t);
        std::function<void(void*&)> instantiate;
        std::function<void*(void*, size_t)> getRawItem;

        template <typename T>
        Type_StdArray(T*)
            : Type{"std::array<>", sizeof(std::array<T, Size>), asset::binary::Type::Array},
              itemType{TypeResolver<T>::get()}
        {
            getSize = [](const void* arrayPtr)
            {
                const auto& vec = *static_cast<const std::array<T, Size>*>(arrayPtr);
                return vec.size();
            };
            getItem = [](const void* arrayPtr, size_t index)
            {
                const auto& vec = *static_cast<const std::array<T, Size>*>(arrayPtr);
                return static_cast<const void*>(&vec.at(index));
            };
            instantiate = [](void*& obj) -> void
            {
                auto& vec = *(std::array<T, Size>*)obj;
            };

            getRawItem = [](void* vecPtr, size_t index) -> void* {
                auto& vec = *(std::array<T, Size>*)vecPtr;
                return &vec[index];
            };
        }

        virtual std::string GetTypeName() const override
        {
            return std::string("std::array<") + itemType->GetTypeName() + ">";
        }

        void Serialize(const void* obj, asset::binary::Object& parentObj, const std::string& fieldName) const override
        {
            SerializeArray(obj, parentObj, fieldName, itemType, getSize, getItem);
        }

        void Deserialize(void* obj, asset::binary::Object& parentObj, const std::string& fieldName) const override
        {
            DeserializeArray(obj, parentObj, fieldName, itemType, instantiate, getRawItem);
        }

        asset::binary::StructLayout GetStructLayout() const override
        {
            asset::binary::StructLayout structLayout = {
                {asset::binary::CreateFixedString32("val"), asset::binary::Type::Array}
            };
            return structLayout;
        }
    };


    template <typename T, size_t Size>
    class TypeResolver<std::array<T, Size>>
    {
    public:
        static Type* get()
        {
            static Type_StdArray<Size> typeDesc{(T*)nullptr};
            return &typeDesc;
        }
    };
}
