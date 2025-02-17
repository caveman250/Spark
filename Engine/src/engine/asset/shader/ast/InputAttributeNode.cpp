#include "InputAttributeNode.h"

#include "NameGenerator.h"
#include "TypeUtil.h"
#include "engine/memory/Arena.h"
#include "engine/memory/ArenaAllocator.h"

namespace se::asset::shader::ast
{
    DEFINE_SPARK_CLASS_BEGIN(InputAttributeNode)
        DEFINE_SERIALIZED_MEMBER(m_Children)
        DEFINE_SERIALIZED_MEMBER(m_Var)
        DEFINE_SERIALIZED_MEMBER(m_Name)
        DEFINE_SERIALIZED_MEMBER(m_Location)
    DEFINE_SPARK_CLASS_END(InputAttributeNode)

    InputAttributeNode::InputAttributeNode(uint8_t location, const Variable& var, const std::string& name)
        : InputNode(var, name)
        , m_Location(location)
    {
    }

    std::string InputAttributeNode::GetDebugString() const
    {
        return std::format("InputAttributeNode - {}, {}, {}", m_Location, TypeUtil::TypeToGlsl(m_Var.type), m_Name);
    }

    void InputAttributeNode::ToGlsl(ShaderCompileContext&, string::ArenaString& outShader) const
    {
        auto alloc = outShader.get_allocator();
        outShader.append(string::ArenaFormat("layout (location={}) in {} {};\n", alloc, m_Location, TypeUtil::TypeToGlsl(m_Var.type), m_Name));
    }

    void InputAttributeNode::ToMtl(ShaderCompileContext&, string::ArenaString& outShader) const
    {
        auto alloc = outShader.get_allocator();
        outShader.append(string::ArenaFormat("device const {}* {} [[buffer({})]]", alloc, TypeUtil::TypeToMtl(m_Var.type), m_Name, m_Location));
    }
}
