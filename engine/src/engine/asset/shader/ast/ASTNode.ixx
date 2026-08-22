module;

#include "spark.h"
#include "engine/string/ArenaString.h"
#include "engine/reflect/Object.h"

export module Spark.Asset.Shader:ASTNode;

namespace se::asset::shader::ast
{
    struct ShaderCompileContext;
    export class ASTNode : public reflect::ObjectBase
    {
        SPARK_CLASS(Abstract)
    public:
        ASTNode() = default;
        ~ASTNode() override = default;
        ASTNode(const ASTNode& rhs);


        virtual const std::shared_ptr<ASTNode>& AddChild(ASTNode* node);
        void DebugPrint(int indent) const;
        virtual std::string GetDebugString() const = 0;
        virtual void ToGlslPreDeclarations(ShaderCompileContext& context, string::ArenaString& outShader);

        virtual void ToGlsl(ShaderCompileContext& context, string::ArenaString& outShader) const = 0;
        virtual void ToMtlPreDeclarations(ShaderCompileContext& context, string::ArenaString& outShader);

        virtual void ToMtl(ShaderCompileContext& context, string::ArenaString& outShader) const = 0;
        virtual void CollectUsedNames(std::map<std::string, std::string>& nameMap) const;
        virtual void ApplyNameRemapping(const std::map<std::string, std::string>& newNames);
        virtual void ForEachChild(const std::function<void(ASTNode*)>& func);

        SPARK_MEMBER(Serialized)
        std::vector<std::shared_ptr<ASTNode>> m_Children;

        ASTNode* m_Parent = nullptr;
    };
}
