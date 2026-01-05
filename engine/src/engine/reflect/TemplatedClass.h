#pragma once

#include "Class.h"
#include "Containers.h"
#include "spark.h"

namespace se::reflect
{
    template <typename T>
    struct TemplatedClass : Class
    {
        TemplatedClass();
    };

    template <typename T>
    TemplatedClass<T>::TemplatedClass()
        : Class()
    {
    }
}
