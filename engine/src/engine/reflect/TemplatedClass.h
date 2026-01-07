#pragma once

#include "Class.h"
#include "Containers.h"
#include "spark.h"

namespace se::reflect
{
    template <typename T>
    struct TemplatedClass : Class
    {
        TemplatedClass() = default;
    };

    template <typename... Ts>
    struct VariadicTemplatedClass : Class
    {
        VariadicTemplatedClass() = default;
    };
}
