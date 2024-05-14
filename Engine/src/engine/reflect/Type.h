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

        Type(const char* name, size_t size, asset::binary::Type binaryType) : name(name), size(size), binaryType(binaryType) {}
        virtual ~Type() {}
        virtual std::string GetTypeName() const { return name; }
        virtual size_t GetTypeSize() const { return size; }
    };
}