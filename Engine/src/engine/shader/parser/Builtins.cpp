#include "Builtins.h"

#include <algorithm>

namespace se::shader::parser
{
    std::array<std::string, 18> Builtins::s_Builtins =
        {
            "int",
            "float",
            "double",
            "half",
            "vec2",
            "vec3",
            "vec4",
            "mat3",
            "mat4",
            "void",
            "layout",
            "in",
            "out",
            "port",
            "uniform",
            "sampler2D",
            "texture"
        };

    bool Builtins::IsBuiltin(const std::string& str)
    {
        return std::ranges::contains(s_Builtins, str);
    }
}
