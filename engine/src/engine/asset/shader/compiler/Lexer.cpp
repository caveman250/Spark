#include "Lexer.h"

#include "Builtins.h"
#include "engine/io/VFS.h"

namespace se::asset::shader::compiler
{
    Lexer::Lexer(const std::string &filePath)
    {
        m_Code = io::VFS::Get().ReadText(filePath).data();
    }

    std::variant<Token, std::string> Lexer::PeekToken(int offset)
    {
        char c = PeekChar(offset);
        switch (c)
        {
            case '\"':
                return ProcessStringLiteral(offset);
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
                return ProcessNumericLiteral(offset);
            case ' ':
                offset += ConsumeWhitespace(offset);
                return PeekToken(offset);
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
            static_cast<uint32_t>(m_CharIdx + m_CurrLinePosOffset)};
            case '<':
            case '>':
            case '=':
            case '+':
            case '-':
            case '*':
            case '/':
                return ProcessComplexSyntax(offset);
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
                return ProcessIdentifierOrBuiltin(offset);
            case '\n':
                m_CurrLine++;
                m_CurrLinePosOffset = -(m_CharIdx + 1);
                if (offset == 0)
                {
                    ConsumeChar();
                }
                else
                {
                    offset++;
                }
                return PeekToken(offset);
            case '\0':
            case '\t':
                if (offset == 0)
                {
                    ConsumeChar();
                }
                else
                {
                    offset++;
                }
                return PeekToken(offset);
            default:
                SPARK_ASSERT(false);
                return std::format("Unrecognized character: {}", c);
        }
    }

    std::variant<Token, std::string> Lexer::PeekTokenAt(int n)
    {
        int offset = 0;
        for (int i = 0; i < n; ++i)
        {
            auto peek = PeekToken(offset);
            if (std::holds_alternative<Token>(peek))
            {
                offset += static_cast<int>(std::get<Token>(peek).value.size());
            }
            else
            {
                return std::format("Found EOF while peeking {0} tokens", n);
            }
        }

        return PeekToken(offset);
    }

    void Lexer::ConsumeToken()
    {
        auto peek = PeekToken();
        if (SPARK_VERIFY(std::holds_alternative<Token>(peek)))
        {
            ConsumeChar(static_cast<int>(std::get<Token>(peek).value.size()));
        }
    }

    bool Lexer::CanPeekChar()
    {
        return m_CharIdx < static_cast<int>(m_Code.size());
    }

    char Lexer::PeekChar()
    {
        return m_Code[m_CharIdx];
    }

    bool Lexer::CanPeekChar(int n)
    {
        return m_CharIdx + n < static_cast<int>(m_Code.size());
    }

    char Lexer::PeekChar(int n)
    {
        return m_Code[m_CharIdx + n];
    }

    void Lexer::ConsumeChar()
    {
        m_CharIdx++;
    }

    void Lexer::ConsumeChar(int n)
    {
        m_CharIdx += n;
    }

    std::variant<Token, std::string> Lexer::ProcessStringLiteral(int offset)
    {
        std::string result = {};
        int lookahead = offset;
        char c = PeekChar(lookahead);

        bool stringEnded = false;
        while (CanPeekChar(lookahead) && !stringEnded)
        {
            stringEnded = c == '\"' && lookahead > offset;
            result += c;
            c = PeekChar(++lookahead);
        }

        if (stringEnded)
        {
            return Token{
                TokenType::StringLiteral, result, m_CurrLine,
                static_cast<uint32_t>(m_CharIdx + lookahead + m_CurrLinePosOffset)
            };
        }
        else
        {
            return "Unexpected end of file while looking for \"";
        }
    }

    std::variant<Token, std::string> Lexer::ProcessNumericLiteral(int offset)
    {
        std::string result = {};
        int lookahead = offset;
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
                static_cast<uint32_t>(m_CharIdx + lookahead + m_CurrLinePosOffset) };
        }
        else
        {
            return "Unexpected end of file while parsing numeric literal";
        }
    }

    Token Lexer::ProcessComplexSyntax(int offset)
    {
        std::string result = {};
        char c1 = PeekChar(offset);
        result += c1;
        if (!CanPeekChar(offset + 1))
        {
            return Token{TokenType::Syntax, result, m_CurrLine,
                static_cast<uint32_t>(m_CharIdx + m_CurrLinePosOffset)};
        }

        char c2 = PeekChar(offset + 1);
        static std::map<char, char> s_AllowedPairs =
        {
            {'<', '='},
            {'>', '='},
            {'*', '='},
            {'/', '='},
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
                static_cast<uint32_t>(m_CharIdx + 1 + m_CurrLinePosOffset) };
            }
        }

        return Token{TokenType::Syntax, result, m_CurrLine,
                static_cast<uint32_t>(m_CharIdx + m_CurrLinePosOffset)};
    }

    std::variant<Token, std::string> Lexer::ProcessIdentifierOrBuiltin(int offset)
    {
        std::string result = {};
        int lookahead = offset;
        char c = PeekChar(lookahead);

        bool isBuiltin = false;
        bool termEnded = false;
        while (CanPeekChar(lookahead) && !termEnded)
        {
            termEnded = !std::isalnum(c) && c != '_';
            if (termEnded)
            {
                if (Builtins::IsBuiltin(result))
                {
                    isBuiltin = true;
                }
            }
            else
            {
                result += c;
                c = PeekChar(++lookahead);
            }
        }

        if (termEnded)
        {
            if (isBuiltin)
            {
                return Token{TokenType::Builtin, result, m_CurrLine,
                static_cast<uint32_t>(m_CharIdx + lookahead + m_CurrLinePosOffset)};
            }
            else
            {
                return Token{TokenType::Identifier, result, m_CurrLine,
                static_cast<uint32_t>(m_CharIdx + lookahead + m_CurrLinePosOffset)};
            }
        }
        else
        {
            return "Unexpected end of file while parsing term";
        }
    }

    int Lexer::ConsumeWhitespace(int offset)
    {
        bool canConsume  = offset == 0;
        std::string result = {};
        char c = PeekChar(offset);

        bool whitespaceEnded = false;
        while (CanPeekChar(offset) && !whitespaceEnded)
        {
            c = PeekChar(offset++);
            whitespaceEnded = !std::isspace(c);
            if (!whitespaceEnded)
            {
                result += c;
            }
        }

        if (canConsume)
        {
            ConsumeChar(static_cast<int>(result.size()));
            return 0;
        }
        else
        {
            return static_cast<int>(result.size());
        }
    }
}
