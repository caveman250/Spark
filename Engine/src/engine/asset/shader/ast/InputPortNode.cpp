#include "InputPortNode.h"

#include "TypeUtil.h"

namespace se::asset::shader::ast
{
    InputPortNode::InputPortNode(const std::string &portName, Type type, const std::string &name)
        : m_PortName(portName)
        , m_Type(type)
        , m_Name(name)
    {
    }

    std::string InputPortNode::GetDebugString() const
    {
        return std::format("InputPortNode - {}, {}, {}", m_PortName, TypeUtil::GetTypeGlsl(m_Type), m_Name);
    }

    void InputPortNode::ToGlsl(string::ArenaString &outShader) const
    {
        auto alloc = outShader.get_allocator();
        outShader.append(string::ArenaFormat("port({0}) in {1} {2};\n", alloc, m_PortName, TypeUtil::GetTypeGlsl(m_Type), m_Name));
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
