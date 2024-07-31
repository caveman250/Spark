#include "InputAttributeNode.h"

#include "NameGenerator.h"
#include "TypeUtil.h"
#include "engine/memory/Arena.h"
#include "engine/memory/ArenaAllocator.h"

namespace se::asset::shader::ast
{
    DEFINE_SPARK_CLASS_BEGIN(InputAttributeNode)
        DEFINE_MEMBER(m_Children)
        DEFINE_MEMBER(m_Type)
        DEFINE_MEMBER(m_Name)
        DEFINE_MEMBER(m_Location)
    DEFINE_SPARK_CLASS_END()

    InputAttributeNode::InputAttributeNode(uint8_t location, AstType::Type type, const std::string& name)
        : InputNode(type, name)
        , m_Location(location)
    {
    }

    std::string InputAttributeNode::GetDebugString() const
    {
        return std::format("InputAttributeNode - {}, {}, {}", m_Location, TypeUtil::GetTypeGlsl(m_Type), m_Name);
    }

    void InputAttributeNode::ToGlsl(string::ArenaString& outShader) const
    {
        auto alloc = outShader.get_allocator();
        outShader.append(string::ArenaFormat("layout (location={}) in {} {};\n", alloc, m_Location, TypeUtil::GetTypeGlsl(m_Type), m_Name));
    }
}
