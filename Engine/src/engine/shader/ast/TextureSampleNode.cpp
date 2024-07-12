#include "TextureSampleNode.h"

namespace se::shader::ast
{
    TextureSampleNode::TextureSampleNode(const std::string &samplerName, const std::string &uvVarName)
        : m_SamplerName(samplerName)
        , m_UVVariableName(uvVarName)
    {

    }

    std::string TextureSampleNode::GetDebugString() const
    {
        return "TextureSampleNode";
    }

    void TextureSampleNode::ToGlsl(string::ArenaString &outShader) const
    {
        auto alloc = outShader.get_allocator();
        outShader += string::ArenaFormat("texture({}, {})", alloc, m_SamplerName, m_UVVariableName);
    }

    void TextureSampleNode::ApplyNameRemapping(const std::map<std::string, std::string> &newNames)
    {
        ASTNode::ApplyNameRemapping(newNames);
        if (newNames.contains(m_SamplerName))
        {
            m_SamplerName = newNames.at(m_SamplerName);
        }

        if (newNames.contains(m_UVVariableName))
        {
            m_UVVariableName = newNames.at(m_UVVariableName);
        }
    }
}