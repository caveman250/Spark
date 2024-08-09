#pragma once

#include "spark.h"

namespace se::asset::shader::compiler
{
    class Builtins
    {
    public:
        static bool IsBuiltin(const std::string& str);
    private:
        static std::array<std::string, 27> s_Builtins;
    };
}