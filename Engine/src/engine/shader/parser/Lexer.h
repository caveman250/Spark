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

        std::variant<Token, std::string> PeekToken(int offset = 0);
        std::variant<Token, std::string> PeekTokenAt(int n);
        void ConsumeToken();

        int GetCharIndex() const { return m_CharIdx; }
        void SetCharIndex(int charIndex) { m_CharIdx = charIndex; }
    private:
        bool CanPeekChar();
        char PeekChar();
        bool CanPeekChar(int n);
        char PeekChar(int n);
        void ConsumeChar();
        void ConsumeChar(int n);

        std::variant<Token, std::string> ProcessStringLiteral(int offset);
        std::variant<Token, std::string> ProcessNumericLiteral(int offset);
        Token ProcessComplexSyntax(int offset);
        std::variant<Token, std::string> ProcessIdentifierOrBuiltin(int offset);

        int ConsumeWhitespace(int offset);

        uint32_t m_CurrLine = 1;
        int m_CurrLinePosOffset = 0;
        std::string m_Code;
        int m_CharIdx = 0;
    };
}
