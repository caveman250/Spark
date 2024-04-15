#pragma once
#include "Lexer.h"
#include "engine/memory/Arena.h"
#include "engine/shader/ast/ShaderStage.h"

namespace se::shader::parser
{
    struct ParseError
    {
        uint32_t line;
        uint32_t pos;
        std::string error;
    };

    class Parser
    {
    public:
        Parser(Lexer  lexer);
        std::variant<ast::ShaderStage, ParseError> Parse();
    private:
        bool ProcessVec2(const Token & token, ParseError& error);
        bool ProcessVec3(const Token & token, ParseError& error);
        bool ProcessVec4(const Token & token, ParseError& error);

        bool ProcessBuiltin(const Token& token, ParseError& outError);
        bool ProcessIdentifier(const Token& token, ParseError& outError);
        bool ProcessTopLevelNumericLiteral(const Token& token, ParseError& outError);
        bool ProcessTopLevelStringLiteral(const Token& token, ParseError& outError);

        bool ProcessSyntax(const Token& token, ParseError& outError);
        bool ProcessLayoutDeclaration(const Token& token, ParseError& outError);
        bool ProcessOutputDeclaration(const Token& token, ParseError& outError);
        bool ProcessFunctionDeclaration(const Token& token, ParseError& outError);
        bool ProcessFunctionArguments(const Token& token, ParseError& outError);
        bool ProcessEndOfFunctionDeclaration(const Token& token, ParseError& outError);
        bool ProcessGLPositionWrite(const Token& token, ParseError& outError);

        bool Expect(const std::vector<TokenType>& allowedTypes, const std::vector<std::string>& allowedValues, ParseError& outError);
        bool ExpectedGetAndConsume(const std::vector<TokenType>& allowedTypes, const std::vector<std::string>& allowedValues, Token& token, ParseError& outError);
        bool ExpectAndConsume(const std::vector<TokenType>& allowedTypes, const std::vector<std::string>& allowedValues, ParseError& outError);

        static bool IsInteger(const std::string& value);
        static bool EnsureInteger(const std::string& value, ParseError& outError);

        ast::ShaderStage m_ShaderStage;
        Lexer m_Lexer;
        memory::Arena m_TempStorage = {};
    };
}
