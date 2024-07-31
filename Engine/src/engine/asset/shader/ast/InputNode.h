#pragma once
#include "ASTNode.h"
#include "Types.h"

namespace se::asset::shader::ast
{
    class InputNode : public ASTNode
    {
        DECLARE_SPARK_CLASS(InputNode)

    public:
        InputNode() = default;
        InputNode(AstType::Type type, const std::string& name);

        std::string GetDebugString() const override;
        void ToGlsl(string::ArenaString& outShader) const override;

        AstType::Type GetType() const;

        const std::string& GetName() const;

    protected:
        AstType::Type m_Type = {};
        std::string m_Name = {};
    };
}
