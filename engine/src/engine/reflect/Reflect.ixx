module;

#include "spark.h"
#include "Enum.h"
#include "Function.h"
#include "System.h"
#include "engine/bits/PackUtil.h"
#include "Reflect_fwd.h"

export module Reflect;
export import :Containers;
export import Reflect.TemplatedClass;
export import :Util;

namespace se::reflect
{
    export template<typename F>
    struct function_traits;

    export template<typename ReturnType, typename ClassType, typename... ArgTypes>
    struct function_traits<ReturnType(ClassType::*)(ArgTypes...)> {
        using return_type = ReturnType;
        using args_type = std::tuple<ArgTypes...>;
    };

    export template <typename T>
    void AddFunctionArgType(std::vector<Type*>& arg_list)
    {
        arg_list.push_back(TypeResolver<T>::Get());
    }

    export template <typename... Args>
    std::vector<Type*> MakeFunctionArgTypesArray([[maybe_unused]] const std::tuple<Args...>& type_tuple)
    {
        std::vector<Type*> ret;
        (AddFunctionArgType<Args>(ret), ...);

        return ret;
    }

    export struct TypeLookup
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