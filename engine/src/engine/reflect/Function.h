#pragma once

#include "spark.h"
#include "TypeResolver.h"

namespace se::reflect
{
    template <typename T>
    void ValidateFunctionArgType(const Class::Function& func, int index, bool& out_valid)
    {
        out_valid &= func.argument_types[index] == TypeResolver<T>::Get();
    }

    template <typename ReturnType, typename ClassType, typename... ArgTypes>
    ReturnType InvokeFunction(ClassType* instance, const std::string& func_name, ArgTypes&&... args)
    {
        auto class_type = ClassResolver<ClassType>::get();
        auto func = std::ranges::find_if(class_type->functions, [&func_name](const Class::Function& func)
        {
            return func.name == func_name;
        });

        if (func == class_type->functions.end())
        {
            debug::Log::Error("Function {} does not exist!", func_name);
            if constexpr(std::is_pointer_v<ReturnType>)
                return nullptr;
            else
                return ReturnType();
        }

        bool valid_args = true;
        int index = 0;
        (ValidateFunctionArgType<ArgTypes>(*func, index++, valid_args), ...);

        if (!valid_args)
        {
            debug::Log::Error("Invalid function arguments!");
            if constexpr(std::is_pointer_v<ReturnType>)
                return nullptr;
            else
                return ReturnType();
        }

        ReturnType (ClassType::*func_ptr)(ArgTypes...) = std::any_cast<ReturnType (ClassType::*)(ArgTypes...)>((*func).func_ptr);

        return ((*instance).*func_ptr)(std::forward<ArgTypes>(args)...);
    }
}
