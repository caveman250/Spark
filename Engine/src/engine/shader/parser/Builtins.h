#pragma once

#include "spark.h"

namespace se::shader::parser
{
    class Builtins
    {
    public:
        static bool IsBuiltin(const std::string& str);
    private:
        static std::array<std::string, 23> s_Builtins;
    };
}