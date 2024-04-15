#pragma once
#include "engine/string/ArenaString.h"

namespace se::shader::ast
{
    class ASTNode
    {
    public:
        std::vector<ASTNode*> Children;
        virtual void ToGlsl(string::ArenaString& outShader) const = 0;
    };
}
