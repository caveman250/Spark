#pragma once

namespace se::asset::shader::compiler
{
    enum class TokenType
    {
        Builtin,
        Identifier,
        NumericLiteral,
        StringLiteral,
        Syntax,
    };

    std::string TokenTypeToString(TokenType token);
    bool IsSyntax(char c);

    struct Token
    {
        TokenType type;
        std::string value;
        uint32_t line;
        uint32_t pos;
    };
}
