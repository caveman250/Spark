#pragma once
#include "ASTNode.h"

namespace se::asset::shader::ast
{
    class PropertyAccessNode : public ASTNode
    {
        DECLARE_SPARK_CLASS(PropertyAccessNode)

    public:
        PropertyAccessNode() {}
        PropertyAccessNode(const std::string& propName);
        std::string GetDebugString() const override;
        void ToGlsl(ShaderCompileContext& context, string::ArenaString& outShader) const override;
        void ToMtl(ShaderCompileContext& context, string::ArenaString& outShader) const override;
    private:
        std::string m_PropertyName = {};
    };
}
