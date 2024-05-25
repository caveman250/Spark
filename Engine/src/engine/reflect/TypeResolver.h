#pragma once

#include "spark.h"
#include "Type.h"

namespace se::reflect
{
    struct Enum;
    struct Class;

    template<typename T>
    Type* getPrimitiveDescriptor();

    struct DefaultResolver
    {
        template<typename T>
        static char func(decltype(&T::Reflection));

        template<typename T>
        static int func(...);

        template<typename T>
        struct IsReflected
        {
            enum
            {
                value = (sizeof(func<T>(nullptr)) == sizeof(char))
            };
        };

        template<typename T, typename std::enable_if<IsReflected<T>::value, int>::type = 0>
        static Type* get()
        {
            return &T::Reflection;
        }

        template<typename T, typename std::enable_if<!IsReflected<T>::value, int>::type = 0>
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
            auto reflectClass = dynamic_cast<Class*>(DefaultResolver::get<T>());
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
            auto reflectEnum = dynamic_cast<Enum*>(DefaultResolver::get<T>());
            SPARK_ASSERT(reflectEnum);
            return reflectEnum;
#else
            return static_cast<Enum*>(DefaultResolver::get<T>());
#endif
        }
    };
}