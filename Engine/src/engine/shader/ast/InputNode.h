#pragma once
#include "ASTNode.h"

namespace se::shader::ast
{
    enum class Type;

    class InputNode : public ASTNode
    {
    public:
        InputNode(Type type, const std::string& name);

        std::string GetDebugString() const override;
        void ToGlsl(string::ArenaString& outShader) const override;

        Type GetType() const;

        const std::string& GetName() const;

    protected:
        Type m_Type;
        std::string m_Name;
    };
}
