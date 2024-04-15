#include "Token.h"

namespace se::shader::parser
{
    std::string TokenTypeToString(TokenType token)
    {
        switch (token)
        {
            case TokenType::Builtin:
                return "Keyword";
            case TokenType::Identifier:
                return "Identifier";
            case TokenType::StringLiteral:
                return "StringLiteral";
            case TokenType::NumericLiteral:
                return "NumericLiteral";
            case TokenType::Syntax:
                return "Syntax";
            default:
                SPARK_ASSERT(false);
                return "";
        }
    }

    bool IsSyntax(char c)
    {
        switch (c)
        {
            case '{':
            case '}':
            case '(':
            case ')':
            case '[':
            case ']':
            case ';':
            case '<':
            case '>':
            case '=':
            case '+':
            case '-':
            case '.':
                return true;
            default:
                return false;
        }
    }
}
