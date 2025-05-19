#include "ElseNode.h"

#include "BinaryExpressionNode.h"
#include "ConstantNode.h"
#include "VariableDeclarationNode.h"
#include "VariableReferenceNode.h"
#include "engine/math/math.h"
#include "ShaderCompileContext.h"

namespace se::asset::shader::ast
{
    ElseNode::ElseNode(const ElseNode& rhs)
            : ASTNode(rhs)
    {
    }

    std::string ElseNode::GetDebugString() const
    {
        return "ElseNode";
    }

    void ElseNode::ToGlsl(ShaderCompileContext& context, string::ArenaString& outShader) const
    {
        auto alloc = outShader.get_allocator();
        outShader += "else\n{\n";

        for (const auto& child : m_Children)
        {
            child->ToGlsl(context, outShader);
        }

        outShader += "}\n";
    }

    void ElseNode::ToMtl(ShaderCompileContext& context, string::ArenaString& outShader) const
    {
        auto alloc = outShader.get_allocator();
        outShader += "else\n{\n";

        for (const auto& child : m_Children)
        {
            child->ToMtl(context, outShader);
        }

        outShader += "}\n";
    }
}
