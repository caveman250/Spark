#include "Lexer.h"

#include "Builtins.h"
#include "engine/io/FileSystem.h"

namespace se::shader::parser
{
    Lexer::Lexer(const std::string &filePath)
    {
        m_Code = io::FileSystem::ReadTextFile(filePath);
    }

    std::variant<Token, std::string> Lexer::PeekToken()
    {
        char c = PeekChar();
        switch (c)
        {
            case '\"':
                return ProcessStringLiteral();
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                return ProcessNumericLiteral();
            case ' ':
                ConsumeWhitespace();
                return PeekToken();
            case '{':
            case '}':
            case '(':
            case ')':
            case '[':
            case ']':
            case ';':
            case ',':
            case '.':
            return Token{TokenType::Syntax, std::string(1, c), m_CurrLine,
            static_cast<uint32_t>(m_CharIdx) + m_CurrLinePosOffset};
            case '<':
            case '>':
            case '=':
            case '+':
            case '-':
            case '*':
                return ProcessComplexSyntax();
            case 'a':
            case 'b':
            case 'c':
            case 'd':
            case 'e':
            case 'f':
            case 'g':
            case 'h':
            case 'i':
            case 'j':
            case 'k':
            case 'l':
            case 'm':
            case 'n':
            case 'o':
            case 'p':
            case 'q':
            case 'r':
            case 's':
            case 't':
            case 'u':
            case 'v':
            case 'w':
            case 'x':
            case 'y':
            case 'z':
            case 'A':
            case 'B':
            case 'C':
            case 'D':
            case 'E':
            case 'F':
            case 'G':
            case 'H':
            case 'I':
            case 'J':
            case 'K':
            case 'L':
            case 'M':
            case 'N':
            case 'O':
            case 'P':
            case 'Q':
            case 'R':
            case 'S':
            case 'T':
            case 'U':
            case 'V':
            case 'W':
            case 'X':
            case 'Y':
            case 'Z':
            case '_':
                return ProcessIdentifierOrBuiltin();
            case '\n':
                m_CurrLine++;
                m_CurrLinePosOffset = -(m_CharIdx + 1);
                ConsumeChar();
                return PeekToken();
            default:
                SPARK_ASSERT(false);
                return std::format("Unrecognized character: {}", c);
        }
    }

    void Lexer::ConsumeToken()
    {
        auto peek = PeekToken();
        if (SPARK_VERIFY(std::holds_alternative<Token>(peek)))
        {
            ConsumeChar(std::get<Token>(peek).value.size());
        }
    }

    bool Lexer::CanPeekChar()
    {
        return m_CharIdx < m_Code.size();
    }

    char Lexer::PeekChar()
    {
        return m_Code[m_CharIdx];
    }

    bool Lexer::CanPeekChar(size_t n)
    {
        return m_CharIdx + n < m_Code.size();
    }

    char Lexer::PeekChar(size_t n)
    {
        return m_Code[m_CharIdx + n];
    }

    void Lexer::ConsumeChar()
    {
        m_CharIdx++;
    }

    void Lexer::ConsumeChar(size_t n)
    {
        m_CharIdx += n;
    }

    std::variant<Token, std::string> Lexer::ProcessStringLiteral()
    {
        std::string result = {};
        int lookahead = 0;
        char c = PeekChar(lookahead);

        bool stringEnded = false;
        while (CanPeekChar(lookahead) && !stringEnded)
        {
            stringEnded = c == '\"' && lookahead > 0;
            result += c;
            c = PeekChar(++lookahead);
        }

        if (stringEnded)
        {
            return Token{
                TokenType::StringLiteral, result, m_CurrLine,
                static_cast<uint32_t>(m_CharIdx) + lookahead + m_CurrLinePosOffset
            };
        }
        else
        {
            return "Unexpected end of file while looking for \"";
        }
    }

    std::variant<Token, std::string> Lexer::ProcessNumericLiteral()
    {
        std::string result = {};
        int lookahead = 0;
        char c = {};
        bool numberEnded = false;
        while (CanPeekChar(lookahead) && !numberEnded)
        {
            c = PeekChar(lookahead++);
            numberEnded = !std::isdigit(c) && c != '.';
            if (!numberEnded)
            {
                result += c;
            }
        }

        if (numberEnded)
        {
            return Token{TokenType::NumericLiteral, result, m_CurrLine,
                static_cast<uint32_t>(m_CharIdx) + lookahead + m_CurrLinePosOffset};
        }
        else
        {
            return "Unexpected end of file while parsing numeric literal";
        }
    }

    Token Lexer::ProcessComplexSyntax()
    {
        std::string result = {};
        char c1 = PeekChar(0);
        result += c1;
        if (!CanPeekChar(1))
        {
            return Token{TokenType::Syntax, result, m_CurrLine,
                static_cast<uint32_t>(m_CharIdx) + m_CurrLinePosOffset};
        }

        char c2 = PeekChar(1);
        static std::map<char, char> s_AllowedPairs =
        {
            {'<', '='},
            {'>', '='},
            {'*', '='},
            {'+', '='},
            {'-', '='},
            {'=', '='},
        };

        for (const auto &[a, b]: s_AllowedPairs)
        {
            if (c1 == a && c2 == b)
            {
                result += c2;
                return Token{TokenType::Syntax, result, m_CurrLine,
                static_cast<uint32_t>(m_CharIdx) + 1 + m_CurrLinePosOffset};
            }
        }

        return Token{TokenType::Syntax, result, m_CurrLine,
                static_cast<uint32_t>(m_CharIdx) + m_CurrLinePosOffset};
    }

    std::variant<Token, std::string> Lexer::ProcessIdentifierOrBuiltin()
    {
        std::string result = {};
        int lookahead = 0;
        char c = PeekChar(lookahead);

        bool isBuiltin = false;
        bool termEnded = false;
        while (CanPeekChar(lookahead) && !termEnded)
        {
            termEnded = !std::isalnum(c) && c != '_';
            if (!termEnded)
            {
                result += c;
                if (Builtins::IsBuiltin(result))
                {
                    isBuiltin = true;
                    termEnded = true;
                }
                c = PeekChar(++lookahead);
            }
        }

        if (termEnded)
        {
            if (isBuiltin)
            {
                return Token{TokenType::Builtin, result, m_CurrLine,
                static_cast<uint32_t>(m_CharIdx) + lookahead + m_CurrLinePosOffset};
            }
            else
            {
                return Token{TokenType::Identifier, result, m_CurrLine,
                static_cast<uint32_t>(m_CharIdx) + lookahead + m_CurrLinePosOffset};
            }
        }
        else
        {
            return "Unexpected end of file while parsing term";
        }
    }

    void Lexer::ConsumeWhitespace()
    {
        std::string result = {};
        int lookahead = 0;
        char c = PeekChar(lookahead);

        bool whitespaceEnded = false;
        while (CanPeekChar(lookahead) && !whitespaceEnded)
        {
            c = PeekChar(lookahead++);
            whitespaceEnded = !std::isspace(c);
            if (!whitespaceEnded)
            {
                result += c;
            }
        }

        ConsumeChar(result.size());
    }
}
