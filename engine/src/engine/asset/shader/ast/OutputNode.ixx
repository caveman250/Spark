module;

#include <spark.h>
#include "engine/string/ArenaString.h"

export module Spark.Asset.Shader:OutputNode;
import :ASTNode;
import :Variable;

namespace se::asset::shader::ast
{
    struct ShaderCompileContext;

    export class OutputNode : public ASTNode
    {
        SPARK_CLASS()

    public:
        OutputNode() = default;
        OutputNode(const Variable& var, const std::string& name);

        std::string GetDebugString() const override;
        void ToGlsl(ShaderCompileContext& context, string::ArenaString& outShader) const override;
        void ToMtl(ShaderCompileContext& context, string::ArenaString& outShader) const override;

        const Variable& GetVar() const;
        void SetVar(const Variable& var) { m_Var = var; }

        const std::string& GetName() const;

    private:
        SPARK_MEMBER(Serialized)
        Variable m_Var = {};

        SPARK_MEMBER(Serialized)
        std::string m_Name = {};
    };
}
