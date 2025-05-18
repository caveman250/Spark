#pragma once
#include "ASTNode.h"
#include "Types.h"

namespace se::asset
{
    class Shader;
}

namespace se::asset::shader::ast
{
    class InputNode;
}

namespace se::asset::shader::ast
{
    class VariableReferenceNode : public ASTNode
    {
        DECLARE_SPARK_CLASS(VariableReferenceNode)

    public:
        VariableReferenceNode() = default;
        VariableReferenceNode(const std::string& name, const Shader& shaderStageAst, int index, const std::string& indexVar);
        std::string GetDebugString() const override;
        void ToGlsl(ShaderCompileContext& context, string::ArenaString& outShader) const override;
        void ToMtl(ShaderCompileContext& context, string::ArenaString& outShader) const override;

        void CollectUsedNames(std::map<std::string, std::string> &nameMap) const override;
        void ApplyNameRemapping(const std::map<std::string, std::string> &newNames) override;

        void SetName(const std::string& name) { m_Name = name; }

        AstType::Type GetType() { return m_Type; }
        const std::string& GetName() { return m_Name; }
        int GetIndex() { return m_Index; }

    private:
        bool IsVertexInput(ShaderCompileContext& context) const;
        bool IsVertexOutput(ShaderCompileContext& context) const;
        bool IsFragmentInput(ShaderCompileContext& context) const;
        bool IsFragmentOutput(const ShaderCompileContext& context) const;

        SPARK_MEMBER(Serialized)
        AstType::Type m_Type = {};

        SPARK_MEMBER(Serialized)
        std::string m_Name = {};

        SPARK_MEMBER(Serialized)
        int m_Index = -1;

        SPARK_MEMBER(Serialized)
        std::string m_IndexVar = "";
    };
}
