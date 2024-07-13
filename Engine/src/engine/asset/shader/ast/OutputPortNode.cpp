#include "OutputPortNode.h"

#include "TypeUtil.h"

namespace se::asset::shader::ast
{
    OutputPortNode::OutputPortNode(const std::string &portName, Type type, const std::string &name)
        : m_PortName(portName)
        , m_Type(type)
        , m_Name(name)
    {
    }

    std::string OutputPortNode::GetDebugString() const
    {
        return std::format("OutputPortNode - {}, {}, {}", m_PortName, TypeUtil::GetTypeGlsl(m_Type), m_Name);
    }

    void OutputPortNode::ToGlsl(string::ArenaString &outShader) const
    {
        auto alloc = outShader.get_allocator();
        outShader.append(string::ArenaFormat("port({0}) out {1} {2};\n", alloc, m_PortName, TypeUtil::GetTypeGlsl(m_Type), m_Name));
    }

    void OutputPortNode::CollectUsedNames(std::map<std::string, std::string> &nameMap) const
    {
        ASTNode::CollectUsedNames(nameMap);
        if (!nameMap.contains(m_Name))
        {
            nameMap.insert({ m_Name, {} });
        }
    }

    void OutputPortNode::ApplyNameRemapping(const std::map<std::string, std::string> &newNames)
    {
        ASTNode::ApplyNameRemapping(newNames);
        if (newNames.contains(m_Name))
        {
            m_Name = newNames.at(m_Name);
        }
    }
}
