#include "OutputPortNode.h"

#include "TypeUtil.h"

namespace se::asset::shader::ast
{
    DEFINE_SPARK_CLASS_BEGIN(OutputPortNode)
        DEFINE_SERIALIZED_MEMBER(m_Children)
        DEFINE_SERIALIZED_MEMBER(m_PortName)
        DEFINE_SERIALIZED_MEMBER(m_Var)
        DEFINE_SERIALIZED_MEMBER(m_Name)
    DEFINE_SPARK_CLASS_END()

    OutputPortNode::OutputPortNode(const std::string &portName, const Variable& var, const std::string &name)
        : m_PortName(portName)
        , m_Var(var)
        , m_Name(name)
    {
    }

    std::string OutputPortNode::GetDebugString() const
    {
        return std::format("OutputPortNode - {}, {}, {}", m_PortName, TypeUtil::GetTypeGlsl(m_Var.type), m_Name);
    }

    void OutputPortNode::ToGlsl(string::ArenaString &outShader) const
    {
        auto alloc = outShader.get_allocator();
        outShader.append(string::ArenaFormat("port({0}) out {1} {2};\n", alloc, m_PortName, TypeUtil::GetTypeGlsl(m_Var.type), m_Name));
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
