#include "InputPortNode.h"
#include "TypeUtil.h"

namespace se::asset::shader::ast
{
    InputPortNode::InputPortNode(const std::string &portName, const Variable& var, const std::string &name)
        : m_PortName(portName)
        , m_Var(var)
        , m_Name(name)
    {
    }

    std::string InputPortNode::GetDebugString() const
    {
        return std::format("InputPortNode - {}, {}, {}", m_PortName, TypeUtil::TypeToGlsl(m_Var.type), m_Name);
    }

    void InputPortNode::ToGlsl(ShaderCompileContext&, string::ArenaString&) const
    {
        SPARK_ASSERT(false, "InputPortNode should not be in final glsl output!");
    }

    void InputPortNode::ToMtl(ShaderCompileContext&, string::ArenaString&) const
    {
        SPARK_ASSERT(false, "InputPortNode should not be in final mtl output!");
    }

    void InputPortNode::CollectUsedNames(std::map<std::string, std::string> &nameMap) const
    {
        ASTNode::CollectUsedNames(nameMap);
        if (!nameMap.contains(m_Name))
        {
            nameMap.insert({ m_Name, {} });
        }
    }

    void InputPortNode::ApplyNameRemapping(const std::map<std::string, std::string> &newNames)
    {
        ASTNode::ApplyNameRemapping(newNames);
        if (newNames.contains(m_Name))
        {
            SetName(newNames.at(m_Name));
        }
    }
}
