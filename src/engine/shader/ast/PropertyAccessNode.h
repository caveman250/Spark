#pragma once
#include "ASTNode.h"

namespace se::shader::ast
{
    class PropertyAccessNode : public ASTNode
    {
    public:
        PropertyAccessNode(const std::string& propName);
        void ToGlsl(string::ArenaString& outShader) const override;
    private:
        std::string m_PropertyName;
    };
}
