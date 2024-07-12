#pragma once
#include "spark.h"
#include "ASTNode.h"
#include "InputNode.h"
#include "Types.h"

namespace se::shader::ast
{
    class InputAttributeNode : public InputNode
    {
    public:
        InputAttributeNode(uint8_t location, Type type, const std::string& name);

        std::string GetDebugString() const override;
        void ToGlsl(string::ArenaString& outShader) const override;

        Type GetType() const { return m_Type; }
        uint8_t GetLocation() const { return m_Location; }
        const std::string& GetName() const { return m_Name; }
    private:
        uint8_t m_Location;
    };
}
