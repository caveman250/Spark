#pragma once

#include "Class.h"
#include "Type.h"
#include "spark.h"

namespace se::reflect
{
    struct Enum;
    struct Class;

    template<typename T>
    Type* GetPrimitiveDescriptor();

    template <typename T>
    concept ClassWithGetReflection = requires
    {
        { T::GetReflection() } -> std::same_as<Class*>;
    };

    struct DefaultResolver
    {
        template<ClassWithGetReflection T>
        static Type* Get()
        {
            return T::GetReflection();
        }

        template<typename T>
        static Type* Get()
        {
            return GetPrimitiveDescriptor<T>();
        }
    };

    template<typename T>
    struct TypeResolver
    {
        static Type* Get()
        {
            return DefaultResolver::Get<T>();
        }
    };

    template<typename T>
    struct ClassResolver
    {
        static Class* Get()
        {
#if !SPARK_DIST
            const auto reflectClass = dynamic_cast<Class*>(DefaultResolver::Get<T>());
            SPARK_ASSERT(reflectClass);
            return reflectClass;
#else
            return static_cast<Class*>(DefaultResolver::Get<T>());
#endif
        }
    };

    template<typename T>
    struct EnumResolver
    {
        static Enum* Get()
        {
#if !SPARK_DIST
            const auto reflectEnum = dynamic_cast<Enum*>(DefaultResolver::Get<T>());
            SPARK_ASSERT(reflectEnum);
            return reflectEnum;
#else
            return static_cast<Enum*>(DefaultResolver::Get<T>());
#endif
        }
    };
}