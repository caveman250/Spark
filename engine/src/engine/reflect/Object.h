#pragma once

#include "spark.h"
#include "TypeResolver.h"
#include "engine/asset/binary/Object.h"

namespace se::reflect
{
    class ObjectBase
    {
    public:
        virtual ~ObjectBase() {}
        static constexpr bool s_IsPOD = false;

        virtual Type* GetReflectType() const = 0;
        virtual void Serialize(const void* obj, asset::binary::Object& parentObj, const std::string& fieldName) = 0;
        virtual void Deserialize(void* obj, asset::binary::Object& parentObj, const std::string& fieldName) = 0;
        virtual asset::binary::StructLayout GetStructLayout(const void*) const = 0;
        virtual std::string GetTypeName() const = 0;
    };
}
