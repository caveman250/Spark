#include "../compiler/Builtins.h"

#include <algorithm>

namespace se::asset::shader::compiler
{
    std::array<std::string, 45> Builtins::s_Builtins =
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
            "setting",
            "sampler2D",
            "texture",
            "length",
            "normalize",
            "smoothstep",
            "clamp",
            "dot",
            "reflect",
            "pow",
            "for",
            "if",
            "else",
            "dfdx",
            "dfdy",
            "SSL_VERTEX",
            "SSL_FRAGMENT",
            "mod",
            "abs",
            "fwidth",
            "inverse",
            "min",
            "max",
            "sqrt",
            "atan2",
            "fract",
            "step",
            "lerp",
            "transpose",
            "mat3"
        };

    bool Builtins::IsBuiltin(const std::string& str)
    {
        return std::ranges::contains(s_Builtins, str);
    }
}
