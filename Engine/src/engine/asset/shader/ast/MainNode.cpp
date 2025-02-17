#include "MainNode.h"

#include "InputNode.h"
#include "ShaderCompileContext.h"
#include "engine/asset/shader/Shader.h"

namespace se::asset::shader::ast
{
    DEFINE_SPARK_CLASS_BEGIN(MainNode)
        DEFINE_SERIALIZED_MEMBER(m_Children)
    DEFINE_SPARK_CLASS_END(MainNode)

    std::string MainNode::GetDebugString() const
    {
        return "MainNode";
    }

    void MainNode::ToGlsl(ShaderCompileContext& context, string::ArenaString& outShader) const
    {
        outShader.append("void main()\n{\n");
        for (const auto& child : m_Children)
        {
           child->ToGlsl(context, outShader);
        }
        outShader.append("}\n");
    }

    void MainNode::ToMtl(ShaderCompileContext& context, string::ArenaString& outShader) const
    {
        if (context.shader.GetType() == ShaderType::Vertex)
        {
            outShader.append("v2f vertex vertexMain(uint vertexId [[vertex_id]]");
            for (const auto& input : context.shader.GetInputs())
            {
                outShader.append(",\n");
                input.second->ToMtl(context, outShader);
            }
            if (! context.shader.GetUniformVariables().empty())
            {
                auto alloc = outShader.get_allocator();
                outShader += string::ArenaFormat(",\ndevice const UniformData& inUniforms [[buffer({})]]", alloc, context.shader.GetInputs().size());
            }
            outShader.append(")\n{\n v2f out;\n");
        }
        else
        {
            outShader.append("half4 fragment fragmentMain(v2f in [[stage_in]])\n{\n");
        }

        for (const auto& child : m_Children)
        {
            child->ToMtl(context, outShader);
        }

        if (context.shader.GetType() == ShaderType::Vertex)
        {
            outShader.append("return out;\n");
        }

        outShader.append("}\n");
    }
}
