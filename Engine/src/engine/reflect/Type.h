#pragma once

#include "spark.h"
#include "engine/asset/binary/Type.h"

namespace se::reflect
{
    struct Type
    {
        const char* name;
        size_t size;
        asset::binary::Type binaryType;
        std::function<void*()> heap_constructor;
        std::function<void*(void*)> inplace_constructor;
        std::function<void*(void*)> heap_copy_constructor;
        std::function<void*(void*, void*)> inplace_copy_constructor;
        std::function<void(void*)> destructor;

        Type(const char* name, size_t size, asset::binary::Type binaryType) : name(name), size(size), binaryType(binaryType) {}
        virtual ~Type() {}
        virtual std::string GetTypeName() const { return name; }
        virtual size_t GetTypeSize() const { return size; }

    };
}