#pragma once
#include "ASTNode.h"
#include "Types.h"

namespace se::asset::shader::ast
{
    class InputPortNode : public ASTNode
    {
        DECLARE_SPARK_CLASS(InputPortNode)

    public:
        InputPortNode() = default;
        InputPortNode(const std::string& portName, AstType::Type type, const std::string& name);

        std::string GetDebugString() const override;
        void ToGlsl(string::ArenaString& outShader) const override;

        AstType::Type GetType() const { return m_Type; }
        const std::string& GetPortName() { return m_PortName; }
        const std::string& GetName() const { return m_Name; }
        void SetName(const std::string& name) { m_Name = name; }

        void CollectUsedNames(std::map<std::string, std::string>& nameMap) const override;
        void ApplyNameRemapping(const std::map<std::string, std::string>& newNames) override;

    private:
        std::string m_PortName = {};
        AstType::Type m_Type = {};
        std::string m_Name = {};
    };
}
