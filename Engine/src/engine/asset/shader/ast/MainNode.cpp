#include "MainNode.h"

#include "InputNode.h"
#include "OutputNode.h"
#include "ShaderCompileContext.h"
#include "TypeUtil.h"
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
        auto alloc = outShader.get_allocator();

        if (context.currentShader->GetType() == ShaderType::Vertex)
        {
            outShader.append("v2f vertex vertexMain(uint vertexId [[vertex_id]]");
            for (const auto& input : context.currentShader->GetInputs())
            {
                outShader.append(",\n");
                input.second->ToMtl(context, outShader);
            }
            if (! context.currentShader->GetUniformVariables().empty())
            {
                outShader += string::ArenaFormat(",\ndevice const UniformData& inUniforms [[buffer({})]]", alloc, context.vertShader->GetInputs().size());
            }
            outShader.append(")\n{\n v2f out;\n");
        }
        else
        {
            outShader.append("FragmentOutput fragment fragmentMain(v2f in [[stage_in]]");
            int index = 0;
            for (const auto& [name, uniform] : context.currentShader->GetUniformVariables())
            {
                if (uniform.type == AstType::Sampler2D)
                {
                    outShader += string::ArenaFormat(",\ntexture2d<float, access::sample> {} [[texture({})]]", alloc, name, index++);
                }
            }
            if (!context.currentShader->GetUniformVariables().empty())
            {
                outShader += ",\nconstant UniformData& inUniforms [[buffer(0)]]";
            }
            outShader.append(")\n{\nFragmentOutput out;\n");
        }

        for (const auto& node: context.currentShader->GetNodes())
        {
            node->ToMtlPreDeclarations(context, outShader);
        }

        for (const auto& child : m_Children)
        {
            child->ToMtl(context, outShader);
        }

        outShader.append("return out;\n");

        outShader.append("}\n");
    }
}
