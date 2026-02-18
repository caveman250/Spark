#pragma once

#include "Class.h"
#include "Type.h"
#include "spark.h"

namespace se::reflect
{
    struct Enum;
    struct Class;

    template<typename T>
    Type* getPrimitiveDescriptor();

    template <typename T>
    concept ClassWithGetReflection = requires
    {
        { T::GetReflection() } -> std::same_as<Class*>;
    };

    struct DefaultResolver
    {
        template<ClassWithGetReflection T>
        static Type* get()
        {
            return T::GetReflection();
        }

        template<typename T>
        static Type* get()
        {
            return getPrimitiveDescriptor<T>();
        }
    };

    template<typename T>
    struct TypeResolver
    {
        static Type* get()
        {
            return DefaultResolver::get<T>();
        }
    };

    template<typename T>
    struct ClassResolver
    {
        static Class* get()
        {
#if !SPARK_DIST
            const auto reflectClass = dynamic_cast<Class*>(DefaultResolver::get<T>());
            SPARK_ASSERT(reflectClass);
            return reflectClass;
#else
            return static_cast<Class*>(DefaultResolver::get<T>());
#endif
        }
    };

    template<typename T>
    struct EnumResolver
    {
        static Enum* get()
        {
#if !SPARK_DIST
            const auto reflectEnum = dynamic_cast<Enum*>(DefaultResolver::get<T>());
            SPARK_ASSERT(reflectEnum);
            return reflectEnum;
#else
            return static_cast<Enum*>(DefaultResolver::get<T>());
#endif
        }
    };
}