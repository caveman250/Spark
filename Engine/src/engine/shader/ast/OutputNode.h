#pragma once
#include "ASTNode.h"

namespace se::shader::ast
{
    enum class Type;

    class OutputNode : public ASTNode
    {
    public:
        OutputNode(Type type, const std::string& name);

        void ToGlsl(string::ArenaString& outShader) const override;

        Type GetType() const;

        const std::string& GetName() const;

    private:
        Type m_Type;
        std::string m_Name;
    };
}
