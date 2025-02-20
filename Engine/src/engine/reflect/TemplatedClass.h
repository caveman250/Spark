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
        std::string GetTypeName(const void*) const override;
    };

    template <typename T>
    TemplatedClass<T>::TemplatedClass()
        : Class()
    {
    }

    template <typename T>
    std::string TemplatedClass<T>::GetTypeName(const void* obj) const
    {
        auto otherType = TypeResolver<T>::get();
        auto otherName = otherType->GetTypeName(nullptr);
        return Class::GetTypeName(obj) + "<" + otherName + ">";
    }
}
