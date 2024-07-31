#pragma once
#include "ASTNode.h"
#include "Types.h"

namespace se::asset::shader::ast
{
    class OutputPortNode : public ASTNode
    {
        DECLARE_SPARK_CLASS(OutputPortNode)

    public:
        OutputPortNode() = default;
        OutputPortNode(const std::string& portName, AstType::Type type, const std::string& name);

        std::string GetDebugString() const override;
        void ToGlsl(string::ArenaString& outShader) const override;

        void CollectUsedNames(std::map<std::string, std::string> &nameMap) const override;
        void ApplyNameRemapping(const std::map<std::string, std::string> &newNames) override;

        AstType::Type GetType() const { return m_Type; }
        const std::string& GetPortName() const { return m_PortName; }
        const std::string& GetName() const { return m_Name; }
        void SetName(const std::string& name) { m_Name = name; }
    private:
        std::string m_PortName = {};
        AstType::Type m_Type = {};
        std::string m_Name = {};
    };
}
