module;

#include <spark.h>
#include "engine/string/ArenaString.h"
#include <map>

export module Spark.Asset.Shader:VariableDeclarationNode;
import :ASTNode;
import :Variable;

namespace se::asset::shader::ast
{
    struct ShaderCompileContext;

    export class VariableDeclarationNode : public ASTNode
    {
        SPARK_CLASS()

    public:
        VariableDeclarationNode() = default;
        VariableDeclarationNode(const std::string& name, const Variable& var);
        std::string GetDebugString() const override;
        void ToGlsl(ShaderCompileContext& context, string::ArenaString& outShader) const override;
        void ToMtl(ShaderCompileContext& context, string::ArenaString& outShader) const override;

        void CollectUsedNames(std::map<std::string, std::string> &nameMap) const override;
        void ApplyNameRemapping(const std::map<std::string, std::string> &newNames) override;

        void SetName(const std::string& name) { m_Name = name; }

        const Variable& GetVar() { return m_Var; }
        const std::string& GetName() { return m_Name; }

    private:
        SPARK_MEMBER(Serialized)
        Variable m_Var = {};

        SPARK_MEMBER(Serialized)
        std::string m_Name = {};
    };
}
