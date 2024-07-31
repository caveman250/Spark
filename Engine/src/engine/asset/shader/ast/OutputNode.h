#pragma once
#include "ASTNode.h"
#include "Types.h"

namespace se::asset::shader::ast
{
    class OutputNode : public ASTNode
    {
        DECLARE_SPARK_CLASS(OutputNode)

    public:
        OutputNode() = default;
        OutputNode(AstType::Type type, const std::string& name);

        std::string GetDebugString() const override;
        void ToGlsl(string::ArenaString& outShader) const override;

        AstType::Type GetType() const;

        const std::string& GetName() const;

    private:
        AstType::Type m_Type = {};
        std::string m_Name = {};
    };
}
