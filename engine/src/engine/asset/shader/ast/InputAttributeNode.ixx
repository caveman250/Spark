module;

#include <spark.h>
#include "engine/string/ArenaString.h"

export module Spark.Asset.Shader:InputAttributeNode;
import :InputNode;

namespace se::asset::shader::ast
{
    struct ShaderCompileContext;
    struct Variable;
    export class InputAttributeNode : public InputNode
    {
        SPARK_CLASS()
    public:
        InputAttributeNode() = default;
        InputAttributeNode(uint8_t location, const Variable& var, const std::string& name);

        std::string GetDebugString() const override;
        void ToGlsl(ShaderCompileContext& context, string::ArenaString& outShader) const override;
        void ToMtl(ShaderCompileContext& context, string::ArenaString& outShader) const override;

        uint8_t GetLocation() const { return m_Location; }
    private:
        SPARK_MEMBER(Serialized)
        uint8_t m_Location = {};
    };
}
