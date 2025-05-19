#include "OutputPortNode.h"

#include "TypeUtil.h"

namespace se::asset::shader::ast
{
    OutputPortNode::OutputPortNode(const std::string &portName, const Variable& var, const std::string &name)
        : m_PortName(portName)
        , m_Var(var)
        , m_Name(name)
    {
    }

    std::string OutputPortNode::GetDebugString() const
    {
        return std::format("OutputPortNode - {}, {}, {}", m_PortName, TypeUtil::TypeToGlsl(m_Var.type), m_Name);
    }

    void OutputPortNode::ToGlsl(ShaderCompileContext&, string::ArenaString&) const
    {
        SPARK_ASSERT(false, "OutputPortNode should not be in final glsl output!");
    }

    void OutputPortNode::ToMtl(ShaderCompileContext&, string::ArenaString&) const
    {
        SPARK_ASSERT(false, "OutputPortNode should not be in final mtl output!");
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
