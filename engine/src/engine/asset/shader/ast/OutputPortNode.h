#pragma once
#include "ASTNode.h"
#include "Variable.h"

namespace se::asset::shader::ast
{
    class OutputPortNode : public ASTNode
    {
        SPARK_CLASS()

    public:
        OutputPortNode() = default;
        OutputPortNode(const std::string& portName, const Variable& type, const std::string& name);

        std::string GetDebugString() const override;
        void ToGlsl(ShaderCompileContext& context, string::ArenaString& outShader) const override;
        void ToMtl(ShaderCompileContext& context, string::ArenaString& outShader) const override;

        void CollectUsedNames(std::map<std::string, std::string> &nameMap) const override;
        void ApplyNameRemapping(const std::map<std::string, std::string> &newNames) override;

        Variable GetVar() const { return m_Var; }
        const std::string& GetPortName() const { return m_PortName; }
        const std::string& GetName() const { return m_Name; }
        void SetName(const std::string& name) { m_Name = name; }
    private:
        SPARK_MEMBER(Serialized)
        std::string m_PortName = {};

        SPARK_MEMBER(Serialized)
        Variable m_Var = {};

        SPARK_MEMBER(Serialized)
        std::string m_Name = {};
    };
}
