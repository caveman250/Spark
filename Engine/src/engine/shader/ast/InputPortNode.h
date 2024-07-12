#pragma once
#include "ASTNode.h"

namespace se::shader::ast
{
    enum class Type;

    class InputPortNode : public ASTNode
    {
    public:
        InputPortNode(const std::string& portName, Type type, const std::string& name);

        std::string GetDebugString() const override;
        void ToGlsl(string::ArenaString& outShader) const override;

        Type GetType() const { return m_Type; }
        const std::string& GetPortName() { return m_PortName; }
        const std::string& GetName() const { return m_Name; }
        void SetName(const std::string& name) { m_Name = name; }

        void CollectUsedNames(std::map<std::string, std::string>& nameMap) const override;
        void ApplyNameRemapping(const std::map<std::string, std::string>& newNames) override;

    private:
        std::string m_PortName;
        Type m_Type;
        std::string m_Name;
    };
}
