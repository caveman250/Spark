module;

#include "Class.h"
#include "spark.h"

export module Reflect.TemplatedClass;

namespace se::reflect
{
    export template <typename T>
    struct TemplatedClass : Class
    {
        TemplatedClass() = default;
    };

    export template <typename... Ts>
    struct VariadicTemplatedClass : Class
    {
        VariadicTemplatedClass() = default;
    };
}
