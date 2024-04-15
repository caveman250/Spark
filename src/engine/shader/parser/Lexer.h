#pragma once
#include <variant>

#include "spark.h"
#include "Token.h"

namespace se::shader::parser
{
    class Lexer
    {
    public:
        Lexer(const std::string& filePath);

        bool Finished() { return !CanPeekChar(); }

        std::variant<Token, std::string> PeekToken();
        void ConsumeToken();
    private:
        bool CanPeekChar();
        char PeekChar();
        bool CanPeekChar(size_t n);
        char PeekChar(size_t n);
        void ConsumeChar();
        void ConsumeChar(size_t n);

        std::variant<Token, std::string> ProcessStringLiteral();
        std::variant<Token, std::string> ProcessNumericLiteral();
        Token ProcessComplexSyntax();
        std::variant<Token, std::string> ProcessIdentifierOrBuiltin();

        void ConsumeWhitespace();

        uint32_t m_CurrLine = 1;
        uint32_t m_CurrLinePosOffset = 0;
        std::string m_Code;
        size_t m_CharIdx = 0;
    };
}
