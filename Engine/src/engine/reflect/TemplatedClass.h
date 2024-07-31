#pragma once

#include "Class.h"
#include "Containers.h"
#include "spark.h"

namespace se::reflect
{
    template <typename T>
    struct TemplatedClass : Class
    {
        TemplatedClass(void (*init)(TemplatedClass*));
        std::string GetTypeName(const void*) const override;
    };

    template <typename T>
    TemplatedClass<T>::TemplatedClass(void(* init)(TemplatedClass*))
        : Class()
    {
        init(this);
    }

    template <typename T>
    std::string TemplatedClass<T>::GetTypeName(const void* obj) const
    {
        return Class::GetTypeName(obj) + "<" + TypeResolver<T>::get()->GetTypeName(nullptr) + ">";
    }
}
