#pragma once
#include "Lexer.h"
#include "engine/memory/Arena.h"
#include "engine/asset/shader/Shader.h"

namespace se::asset::shader::compiler
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
        Parser(Lexer lexer);
        std::variant<Shader, ParseError> Parse();

    private:
        bool ProcessBuiltin(const Token& token, ast::AstType::Type& returnType, ParseError& outError);
        bool ProcessNumericLiteral(const Token& token, ParseError& outError);
        bool ProcessStringLiteral(const Token& token, ParseError& outError);
        bool ProcessSyntax(const Token& token, ParseError& outError);

        bool ProcessPortDeclaration(const Token& token, ast::AstType::Type& returnType, ParseError& outError);
        bool ProcessUniformDeclaration(const Token& token, ast::AstType::Type& returnType, ParseError& outError);
        bool ProcessTextureRead(const Token& token, ParseError& outError);
        bool ProcessLengthFunc(const Token& token, ParseError& outError);
        bool ProcessPowFunc(const Token& token, ParseError& outError);
        bool ProcessNormalizeFunc(const Token& token, ast::AstType::Type& returnType, ParseError& outError);
        bool ProcessReflectFunc(const Token& token, ast::AstType::Type& returnType, ParseError& outError);
        bool ProcessClampFunc(const Token& token, ParseError& outError);
        bool ProcessDotFunc(const Token& token, ParseError& outError);
        bool ProcessVariableDeclaration(const Token& token, ParseError& outError);
        bool ProcessFunctionDeclaration(const Token& token, ParseError& outError);
        bool ProcessEndOfFunctionDeclaration(const Token& token, ParseError& outError);
        bool ProcessPropertyAccess(const Token& token, ast::AstType::Type& returnType, ParseError& outError);

        bool ProcessVec2(const Token& token, ParseError& error);
        bool ProcessVec3(const Token& token, ParseError& error);
        bool ProcessVec4(const Token& token, ParseError& error);

        bool ProcessExpression(ast::AstType::Type& outType, ParseError& outError);

        bool Peek(int offset, const std::vector<TokenType>& allowedTypes, const std::vector<std::string>& allowedValues);
        bool Peek(const std::vector<TokenType>& allowedTypes, const std::vector<std::string>& allowedValues);
        bool Peek(const std::vector<TokenType>& allowedTypes, const std::vector<std::string>& allowedValues, Token& outToken);
        bool Peek(int offset, const std::vector<TokenType>& allowedTypes, const std::vector<std::string>& allowedValues, Token& outToken);
        bool IsNextTokenBinaryOperator();
        bool Expect(const std::vector<TokenType>& allowedTypes, const std::vector<std::string>& allowedValues,
                    ParseError& outError);
        bool ExpectedGet(const std::vector<TokenType>& allowedTypes,
                           const std::vector<std::string>& allowedValues, Token& token, ParseError& outError);
        bool ExpectedGetAndConsume(const std::vector<TokenType>& allowedTypes,
                                   const std::vector<std::string>& allowedValues, Token& token, ParseError& outError);
        bool ExpectAndConsume(const std::vector<TokenType>& allowedTypes, const std::vector<std::string>& allowedValues,
                              ParseError& outError);

        static bool IsInteger(const std::string& value);
        static bool EnsureInteger(const std::string& value, ParseError& outError);
        bool EnsureTypeAndConsume(std::vector<ast::AstType> allowedTypes, ParseError& outError);

        Shader m_Shader;
        Lexer m_Lexer;
    };
}
