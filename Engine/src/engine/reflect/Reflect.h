#pragma once

#include "spark.h"
#include "TemplatedClass.h"
#include "Enum.h"
#include "Containers.h"
#include "engine/bits/PackUtil.h"
#include "Reflect_fwd.h"

namespace se::reflect
{
    struct TypeLookup
    {
        static Type* GetType(const std::string& type) { SPARK_ASSERT(GetTypeMap().contains(type)); return GetTypeMap().at(type); }
        // Warning: Accessed during static initialization.
        static std::unordered_map<std::string, Type*>& GetTypeMap()
        {
            static std::unordered_map<std::string, Type*> typeMap;
            return typeMap;
        }
    };
}