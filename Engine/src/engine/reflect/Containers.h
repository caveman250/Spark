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

        template <typename T>
        Type_StdVector(T*)
                : Type{ "std::vector<>", sizeof(std::vector<T>), asset::binary::Type::Array },
                  itemType{ TypeResolver<T>::get() }
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
        }

        virtual std::string GetTypeName() const override
        {
            return std::string("std::vector<") + itemType->GetTypeName() + ">";
        }
    };

    template <typename T>
    class TypeResolver<std::vector<T> >
    {
    public:
        static Type* get()
        {
            static Type_StdVector typeDesc{(T*) nullptr};
            return &typeDesc;
        }
    };

    template <size_t Size>
    struct Type_StdArray : Type
    {
        Type* itemType;
        size_t (*getSize)(const void*);
        const void* (*getItem)(const void*, size_t);
        template <typename T>
        Type_StdArray(T*)
                : Type{ "std::array<>", sizeof(std::array<T, Size>), asset::binary::Type::Array },
                  itemType{ TypeResolver<T>::get() }
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
        }
        virtual std::string GetTypeName() const override
        {
            return std::string("std::array<") + itemType->GetTypeName() + ">";
        }
    };


    template <typename T, size_t Size>
    class TypeResolver<std::array<T, Size>>
    {
    public:
        static Type* get()
        {
            static Type_StdArray<Size> typeDesc{(T*) nullptr};
            return &typeDesc;
        }
    };
}