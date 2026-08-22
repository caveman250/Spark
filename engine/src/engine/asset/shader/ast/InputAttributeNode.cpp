module;
#include "engine/memory/ArenaAllocator.h"
#include "engine/string/ArenaString.h"

module Spark.Asset.Shader;
import :InputAttributeNode;
import :TypeUtil;

namespace se::asset::shader::ast
{
    InputAttributeNode::InputAttributeNode(const uint8_t location, const Variable& var, const std::string& name)
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
