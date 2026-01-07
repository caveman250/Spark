#pragma once

#include "spark.h"
#include "TemplatedClass.h"
#include "Enum.h"
#include "Containers.h"
#include "Function.h"
#include "System.h"
#include "engine/bits/PackUtil.h"
#include "Reflect_fwd.h"

namespace se::reflect
{
    template<typename F>
    struct function_traits;

    template<typename ReturnType, typename ClassType, typename... ArgTypes>
    struct function_traits<ReturnType(ClassType::*)(ArgTypes...)> {
        using return_type = ReturnType;
        using args_type = std::tuple<ArgTypes...>;
    };

    template <typename T>
    void AddFunctionArgType(std::vector<Type*>& arg_list)
    {
        arg_list.push_back(TypeResolver<T>::get());
    }

    template <typename... Args>
    std::vector<Type*> MakeFunctionArgTypesArray([[maybe_unused]] const std::tuple<Args...>& type_tuple)
    {
        std::vector<Type*> ret;
        (AddFunctionArgType<Args>(ret), ...);

        return ret;
    }

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