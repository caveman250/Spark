#pragma once
#include "Lexer.h"
#include "engine/asset/shader/Shader.h"
#include "engine/asset/shader/ast/TypeUtil.h"

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
        bool ProcessBuiltin(const Token& token, ast::AstType& returnType, ParseError& outError);
        bool ProcessNumericLiteral(const Token& token, ParseError& outError);
        bool ProcessStringLiteral(const Token& token, ParseError& outError);
        bool ProcessSyntax(const Token& token, ParseError& outError);

        bool ProcessPortDeclaration(const Token& token, ast::AstType& returnType, ParseError& outError);
        bool ProcessUniformDeclaration(const Token& token, ast::AstType& returnType, ParseError& outError);
        bool ProcessSettingDeclaration(const Token& token, ast::AstType& returnType, ParseError& outError);
        bool ProcessTextureRead(const Token& token, ParseError& outError);
        bool ProcessVariableDeclaration(const Token& token, ParseError& outError);
        bool ProcessFunctionDeclaration(const Token& token, ParseError& outError);
        bool ProcessEndOfFunctionDeclaration(const Token& token, ParseError& outError);
        bool ProcessPropertyAccess(const Token& token, ast::AstType& returnType, ParseError& outError);

        template <typename T, size_t Size>
        bool ProcessFunc(const Token& token,
            ParseError& outError,
            std::array<ast::AstType, Size> argTypes);

        template <typename T, size_t Size>
        bool ProcessFuncVariableReturn(const Token& token,
            ast::AstType& returnType,
            ParseError& outError,
            std::array<ast::AstType, Size> argTypes);

        bool ProcessVec2(const Token& token, ParseError& error);
        bool ProcessVec3(const Token& token, ParseError& error);
        bool ProcessVec4(const Token& token, ParseError& error);

        bool ProcessExpression(ast::AstType& outType, ParseError& outError);
        bool ProcessForLoop(ParseError& outError);
        bool ProcessIfStatement(ParseError& outError);
        bool ProcessElseStatement(ParseError& outError);

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

    template<typename T, size_t Size>
    bool Parser::ProcessFunc(const Token& token,
        ParseError& outError,
        std::array<ast::AstType, Size> argTypes)
    {
        ast::AstType dummy;
        return ProcessFuncVariableReturn<T>(token, dummy, outError, argTypes);
    }

    template<typename T, size_t Size>
    bool Parser::ProcessFuncVariableReturn(const Token& token,
        ast::AstType& returnType,
        ParseError& outError,
        std::array<ast::AstType, Size> argTypes)
    {
        if (!ExpectAndConsume({TokenType::Syntax}, {"("}, outError))
        {
            return false;
        }

        auto max = m_Shader.AddNode<T>();
        m_Shader.PushScope(max);
        int argumentsAccountedFor = 0;
        Token nextToken;
        if (argTypes.size() > 0)
        {
            while (true)
            {
                if (Peek({TokenType::Syntax}, {")"}))
                {
                    break;
                }

                ast::AstType argType;
                if (!ProcessExpression(argType, outError))
                {
                    return false;
                }

                if ((argType & argTypes[argumentsAccountedFor]) == ast::AstType::None)
                {
                    outError = {
                        token.line, token.pos,
                        std::format("Unexpected type {}", ast::TypeUtil::TypeToGlsl(argType))
                    };
                    return false;
                }

                argumentsAccountedFor++;

                // TODO This is not great
                if (returnType == ast::AstType::Invalid)
                {
                    returnType = argType;
                }

                if (Peek({TokenType::Syntax}, {")"}))
                {
                    break;
                }

                if (!ExpectedGetAndConsume({TokenType::Syntax}, {","}, nextToken, outError))
                {
                    return false;
                }
            }
        }

        if (!ExpectedGetAndConsume({TokenType::Syntax}, {")"}, nextToken, outError))
        {
            return false;
        }

        if (argumentsAccountedFor != argTypes.size())
        {
            outError = {
                nextToken.line, nextToken.pos, std::format("Incorrect number of arguments for function call {}", token.value)
            };
            return false;
        }

        m_Shader.PopScope();

        return true;
    }
}
