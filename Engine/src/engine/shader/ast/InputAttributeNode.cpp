#include "InputAttributeNode.h"

#include "NameGenerator.h"
#include "TypeUtil.h"
#include "engine/memory/Arena.h"
#include "engine/memory/ArenaAllocator.h"

namespace se::shader::ast
{
    InputAttributeNode::InputAttributeNode(uint8_t location, Type type, const std::string& name)
        : InputNode(type, name)
        , m_Location(location)
    {
    }

    void InputAttributeNode::ToGlsl(string::ArenaString& outShader) const
    {
        auto alloc = outShader.get_allocator();
        outShader.append(string::ArenaFormat("layout (location={}) in {} {};\n", alloc, m_Location, TypeUtil::GetTypeGlsl(m_Type), m_Name));
    }
}
