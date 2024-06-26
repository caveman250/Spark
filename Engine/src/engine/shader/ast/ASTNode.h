#pragma once
#include "engine/string/ArenaString.h"

namespace se::shader::ast
{
    class ASTNode
    {
    public:
        virtual ~ASTNode() = default;
        std::vector<ASTNode*> m_Children;
        virtual void ToGlsl(string::ArenaString& outShader) const = 0;
        virtual void CollectUsedNames(std::map<std::string, std::string>& nameMap) const;
        virtual void ApplyNameRemapping(const std::map<std::string, std::string>& newNames);
    };
}
