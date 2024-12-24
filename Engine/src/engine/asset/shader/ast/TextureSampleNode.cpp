#include "TextureSampleNode.h"

namespace se::asset::shader::ast
{
    DEFINE_SPARK_CLASS_BEGIN(TextureSampleNode)
        DEFINE_SERIALIZED_MEMBER(m_Children)
        DEFINE_SERIALIZED_MEMBER(m_SamplerName)
        DEFINE_SERIALIZED_MEMBER(m_UVVariableName)
    DEFINE_SPARK_CLASS_END()

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