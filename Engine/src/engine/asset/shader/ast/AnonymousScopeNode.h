#pragma once
#include "ASTNode.h"

namespace se::asset::shader::ast
{
    class AnonymousScopeNode : public ASTNode
    {
        DECLARE_SPARK_CLASS(AnonymousScopeNode)
        
        std::string GetDebugString() const override;
        void ToGlsl(string::ArenaString &outShader) const override;
    };
}
