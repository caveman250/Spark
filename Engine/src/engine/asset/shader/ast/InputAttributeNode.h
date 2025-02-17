#pragma once
#include "spark.h"
#include "ASTNode.h"
#include "InputNode.h"
#include "Types.h"

namespace se::asset::shader::ast
{
    class InputAttributeNode : public InputNode
    {
        DECLARE_SPARK_CLASS(InputAttributeNode)

    public:
        InputAttributeNode() = default;
        InputAttributeNode(uint8_t location, const Variable& var, const std::string& name);

        std::string GetDebugString() const override;
        void ToGlsl(ShaderCompileContext& context, string::ArenaString& outShader) const override;
        void ToMtl(ShaderCompileContext& context, string::ArenaString& outShader) const override;

        uint8_t GetLocation() const { return m_Location; }
    private:
        uint8_t m_Location = {};
    };
}
