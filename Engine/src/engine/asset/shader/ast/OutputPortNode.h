#pragma once
#include "ASTNode.h"

namespace se::asset::shader::ast
{
    enum class Type;

    class OutputPortNode : public ASTNode
    {
    public:
        OutputPortNode(const std::string& portName, Type type, const std::string& name);

        std::string GetDebugString() const override;
        void ToGlsl(string::ArenaString& outShader) const override;

        void CollectUsedNames(std::map<std::string, std::string> &nameMap) const override;
        void ApplyNameRemapping(const std::map<std::string, std::string> &newNames) override;

        Type GetType() const { return m_Type; }
        const std::string& GetPortName() const { return m_PortName; }
        const std::string& GetName() const { return m_Name; }
        void SetName(const std::string& name) { m_Name = name; }
    private:
        std::string m_PortName;
        Type m_Type;
        std::string m_Name;
    };
}
