#pragma once
#include <variant>

#include "spark.h"
#include "Token.h"

namespace se::asset::shader::compiler
{
    class Lexer
    {
    public:
        explicit Lexer(const std::string& filePath);

        bool Finished() const { return !CanPeekChar(); }

        std::variant<Token, std::string> PeekToken(int offset = 0);
        std::variant<Token, std::string> PeekTokenAt(int n);
        void ConsumeToken();

        int GetCharIndex() const { return m_CharIdx; }
        void SetCharIndex(const int charIndex) { m_CharIdx = charIndex; }
    private:
        bool CanPeekChar() const;
        char PeekChar() const;
        bool CanPeekChar(int n) const;
        char PeekChar(int n) const;
        void ConsumeChar();
        void ConsumeChar(int n);

        std::variant<Token, std::string> ProcessStringLiteral(int offset) const;
        std::variant<Token, std::string> ProcessNumericLiteral(int offset) const;
        Token ProcessComplexSyntax(int offset) const;
        std::variant<Token, std::string> ProcessIdentifierOrBuiltin(int offset) const;

        int ConsumeWhitespace(int offset);

        uint32_t m_CurrLine = 1;
        int m_CurrLinePosOffset = 0;
        std::string m_Code;
        int m_CharIdx = 0;
    };
}
