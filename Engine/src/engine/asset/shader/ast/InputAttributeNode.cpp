#include "InputAttributeNode.h"

#include "NameGenerator.h"
#include "TypeUtil.h"
#include "engine/memory/Arena.h"
#include "engine/memory/ArenaAllocator.h"

namespace se::asset::shader::ast
{
    DEFINE_SPARK_CLASS_BEGIN(InputAttributeNode)
        DEFINE_MEMBER(m_Children)
        DEFINE_MEMBER(m_Var)
        DEFINE_MEMBER(m_Name)
        DEFINE_MEMBER(m_Location)
    DEFINE_SPARK_CLASS_END()

    InputAttributeNode::InputAttributeNode(uint8_t location, const Variable& var, const std::string& name)
        : InputNode(var, name)
        , m_Location(location)
    {
    }

    std::string InputAttributeNode::GetDebugString() const
    {
        return std::format("InputAttributeNode - {}, {}, {}", m_Location, TypeUtil::GetTypeGlsl(m_Var.type), m_Name);
    }

    void InputAttributeNode::ToGlsl(string::ArenaString& outShader) const
    {
        auto alloc = outShader.get_allocator();
        std::string arrayTag = "";
        if (!m_Var.arraySizeVariable.empty())
        {
            arrayTag = std::format("[{}]", m_Var.arraySizeVariable);
        }
        else if (m_Var.arraySizeConstant != 0)
        {
            arrayTag = std::format("[{}]", m_Var.arraySizeConstant);
        }
        outShader.append(string::ArenaFormat("layout (location={}) in {} {}{};\n", alloc, m_Location, TypeUtil::GetTypeGlsl(m_Var.type), m_Name, arrayTag));
    }
}
