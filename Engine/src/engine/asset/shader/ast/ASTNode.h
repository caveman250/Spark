#pragma once
#include "engine/reflect/Reflect.h"
#include "engine/string/ArenaString.h"

namespace se::asset::shader::ast
{
    class ASTNode : public reflect::ObjectBase
    {
        DECLARE_SPARK_CLASS(ASTNode)
    public:
        virtual ~ASTNode() = default;

        void DebugPrint(int indent) const;
        virtual std::string GetDebugString() const = 0;
        virtual void ToGlsl(string::ArenaString& outShader) const = 0;
        virtual void CollectUsedNames(std::map<std::string, std::string>& nameMap) const;
        virtual void ApplyNameRemapping(const std::map<std::string, std::string>& newNames);

        std::vector<std::shared_ptr<ASTNode>> m_Children;
    };
}
