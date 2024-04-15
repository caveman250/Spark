#pragma once
#include "spark.h"
#include "ASTNode.h"
#include "Types.h"

namespace se::shader::ast
{
    class InputAttributeNode : public ASTNode
    {
    public:
        InputAttributeNode(uint8_t location, Type type, const std::string& name);

        void ToGlsl(string::ArenaString& outShader) const override;

        Type GetType() const { return m_Type; }
        uint8_t GetLocation() const { return m_Location; }
        const std::string& GetName() const { return m_Name; }
    private:
        uint8_t m_Location;
        Type m_Type;
        std::string m_Name;
    };
}
