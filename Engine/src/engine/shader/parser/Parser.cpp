#include "Parser.h"

#include "engine/shader/ast/AnonymousScopeNode.h"
#include "engine/shader/ast/AssignmentNode.h"
#include "engine/shader/ast/ConstantNode.h"
#include "engine/shader/ast/EndOfExpressionNode.h"
#include "engine/shader/ast/InputAttributeNode.h"
#include "engine/shader/ast/InputPortNode.h"
#include "engine/shader/ast/MainNode.h"
#include "engine/shader/ast/OperatorNode.h"
#include "engine/shader/ast/Operators.h"
#include "engine/shader/ast/OutputNode.h"
#include "engine/shader/ast/OutputPortNode.h"
#include "engine/shader/ast/Types.h"
#include "engine/shader/ast/TypeUtil.h"
#include "engine/shader/ast/VariableDeclarationNode.h"
#include "engine/shader/ast/VariableReferenceNode.h"
#include "engine/shader/ast/Vec2Node.h"
#include "engine/shader/ast/Vec3Node.h"
#include "engine/shader/ast/Vec4Node.h"

namespace se::shader::ast
{
    class OutputPortNode;
    class InputPortNode;
}

namespace se::shader::parser
{
    Parser::Parser(Lexer lexer, memory::Arena* arena)
        : m_Lexer(std::move(lexer))
          , m_TempStorage(arena)
    {
    }

    std::variant<ast::ShaderStage, ParseError> Parser::Parse()
    {
        ParseError error;
        while (!m_Lexer.Finished())
        {
            auto nextToken = m_Lexer.PeekToken();
            if (std::holds_alternative<Token>(nextToken))
            {
                Token token = std::get<Token>(nextToken);
                m_Lexer.ConsumeToken();

                switch (token.type)
                {
                case TokenType::Builtin:
                    if (!ProcessBuiltin(token, error))
                    {
                        return error;
                    }
                    break;
                case TokenType::Identifier:
                    if (!ProcessIdentifier(token, error))
                    {
                        return error;
                    }
                    break;
                case TokenType::NumericLiteral:
                    if (!ProcessNumericLiteral(token, error))
                    {
                        return error;
                    }
                    break;
                case TokenType::StringLiteral:
                    if (!ProcessStringLiteral(token, error))
                    {
                        return error;
                    }
                    break;
                case TokenType::Syntax:
                    if (!ProcessSyntax(token, error))
                    {
                        return error;
                    }
                    break;
                default:
                    SPARK_ASSERT(false);
                    break;
                }
            }
            else
            {
                debug::Log::Error(std::get<std::string>(nextToken));
            }
        }

        return m_ShaderStage;
    }

    bool Parser::ProcessVec2(const Token&, ParseError& outError)
    {
        Token nextToken;
        if (!ExpectedGetAndConsume({TokenType::Syntax}, {"("}, nextToken, outError))
        {
            return false;
        }

        auto* vec4 = m_TempStorage->Alloc<ast::Vec2Node>();
        m_ShaderStage.AddNode(vec4);
        m_ShaderStage.PushScope(vec4);
        int componentsAccountedFor = 0;
        while (componentsAccountedFor < 2)
        {
            ast::Type argType;
            if (!ProcessArgument(nextToken, argType, outError))
            {
                return false;
            }

            switch (argType)
            {
            case ast::Type::Float:
                componentsAccountedFor++;
                break;
            case ast::Type::Vec2:
                componentsAccountedFor += 2;
                break;
            case ast::Type::Vec3:
            case ast::Type::Vec4:
            case ast::Type::Mat3:
            case ast::Type::Mat4:
            case ast::Type::Void:
            case ast::Type::Invalid:
                outError = {nextToken.line, nextToken.pos, std::format("Unexpected type {}", ast::TypeUtil::GetTypeGlsl(argType)) };
                return false;
            }

            if (componentsAccountedFor < 2)
            {
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

        m_ShaderStage.PopScope();

        return true;
    }

    bool Parser::ProcessVec3(const Token&, ParseError& outError)
    {
        Token nextToken;
        if (!ExpectedGetAndConsume({TokenType::Syntax}, {"("}, nextToken, outError))
        {
            return false;
        }

        auto* vec4 = m_TempStorage->Alloc<ast::Vec3Node>();
        m_ShaderStage.AddNode(vec4);
        m_ShaderStage.PushScope(vec4);
        int componentsAccountedFor = 0;
        while (componentsAccountedFor < 3)
        {
            ast::Type argType;
            if (!ProcessArgument(nextToken, argType, outError))
            {
                return false;
            }

            switch (argType)
            {
            case ast::Type::Float:
                componentsAccountedFor++;
                break;
            case ast::Type::Vec2:
                componentsAccountedFor += 2;
                break;
            case ast::Type::Vec3:
                componentsAccountedFor += 3;
                break;
            case ast::Type::Vec4:
            case ast::Type::Mat3:
            case ast::Type::Mat4:
            case ast::Type::Void:
            case ast::Type::Invalid:
                outError = {nextToken.line, nextToken.pos, std::format("Unexpected type {}", ast::TypeUtil::GetTypeGlsl(argType)) };
                return false;
            }

            if (componentsAccountedFor < 3)
            {
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

        m_ShaderStage.PopScope();

        return true;
    }

    bool Parser::ProcessVec4(const Token&, ParseError& outError)
    {
        Token nextToken;
        if (!ExpectedGetAndConsume({TokenType::Syntax}, {"("}, nextToken, outError))
        {
            return false;
        }

        auto* vec4 = m_TempStorage->Alloc<ast::Vec4Node>();
        m_ShaderStage.AddNode(vec4);
        m_ShaderStage.PushScope(vec4);
        int componentsAccountedFor = 0;
        while (componentsAccountedFor < 4)
        {
            ast::Type argType;
            if (!ProcessArgument(nextToken, argType, outError))
            {
                return false;
            }

            switch (argType)
            {
            case ast::Type::Float:
                componentsAccountedFor++;
                break;
            case ast::Type::Vec2:
                componentsAccountedFor += 2;
                break;
            case ast::Type::Vec3:
                componentsAccountedFor += 3;
                break;
            case ast::Type::Vec4:
                componentsAccountedFor += 4;
                break;
            case ast::Type::Mat3:
            case ast::Type::Mat4:
            case ast::Type::Void:
            case ast::Type::Invalid:
                outError = {nextToken.line, nextToken.pos, std::format("Unexpected type {}", ast::TypeUtil::GetTypeGlsl(argType)) };
                return false;
            }

            if (componentsAccountedFor < 4)
            {
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

        m_ShaderStage.PopScope();

        return true;
    }

    bool Parser::ProcessBuiltin(const Token& token, ParseError& outError)
    {
        bool isPossibleVariableDec = token.value == "vec2"
            || token.value == "vec3"
            || token.value == "vec4"
            || token.value == "float";

        if (isPossibleVariableDec)
        {
            auto peek = m_Lexer.PeekToken();
            if (std::holds_alternative<Token>(peek))
            {
                auto nextToken = std::get<Token>(peek);
                if (nextToken.type == TokenType::Identifier)
                {
                    peek = m_Lexer.PeekTokenAt(1);
                    if (std::holds_alternative<Token>(peek))
                    {
                        nextToken = std::get<Token>(peek);
                        if (nextToken.type == TokenType::Syntax && (nextToken.value == "=" || nextToken.value == ";"))
                        {
                            return ProcessVariableDeclaration(token, outError);
                        }
                    }
                }
            }
        }


        if (token.value == "void")
        {
            return ProcessFunctionDeclaration(token, outError);
        }
        else if (token.value == "vec2")
        {
            return ProcessVec2(token, outError);
        }
        else if (token.value == "vec3")
        {
            return ProcessVec3(token, outError);
        }
        else if (token.value == "vec4")
        {
            return ProcessVec4(token, outError);
        }
        else if (token.value == "port")
        {
            return ProcessPortDeclaration(token, outError);
        }
        else if (token.value == "uniform")
        {
            return ProcessUniformDeclaration(token, outError);
        }

        outError = {token.line, token.pos, std::format("Unexpected token {}", token.value)};
        SPARK_ASSERT(false);
        return false;
    }

    bool Parser::ProcessIdentifier(const Token& token, ParseError& outError)
    {
        ast::Type type;
        if (!m_ShaderStage.FindVariable(token.value, type))
        {
            outError = {token.line, token.pos, std::format("Undefined variable: {}", token.value)};
            return false;
        }

        m_ShaderStage.AddNode(m_TempStorage->Alloc<ast::VariableReferenceNode>(token.value, m_ShaderStage));

        Token nextToken;
        if (!ExpectedGetAndConsume({TokenType::Syntax}, {"=", "."}, nextToken, outError))
        {
            return false;
        }

        if (nextToken.value == "=")
        {
            if (!ProcessAssignment(nextToken, type, outError))
            {
                return false;
            }

            if (!ExpectedGetAndConsume({TokenType::Syntax},
                {";", "*", "/", "+", "-", "*=", "/=", "+=", "-="},
                nextToken, outError))
            {
                return false;
            }

            if (!ProcessSyntax(nextToken, outError))
            {
                return false;
            }

            return true;
        }

        outError = {
            nextToken.line, nextToken.pos,
            std::format("Parser::ProcessIdentifier - Not Implemented: {}", nextToken.value)
        };
        return false;
    }

    bool Parser::ProcessNumericLiteral(const Token& token, ParseError&)
    {
        m_ShaderStage.AddNode(m_TempStorage->Alloc<ast::ConstantNode<float>>(std::stof(token.value)));
        return true;
    }

    bool Parser::ProcessStringLiteral(const Token& token, ParseError&)
    {
        m_ShaderStage.AddNode(m_TempStorage->Alloc<ast::ConstantNode<std::string>>(token.value));
        return true;
    }

    bool Parser::ProcessSyntax(const Token& token, ParseError& outError)
    {
        if (token.value == "}")
        {
            m_ShaderStage.PopScope();
            return true;
        }
        else if (token.value == "{")
        {
            auto scope = m_TempStorage->Alloc<ast::AnonymousScopeNode>();
            m_ShaderStage.AddNode(scope);
            m_ShaderStage.PushScope(scope);
            return true;
        }
        else if (token.value == ";")
        {
            m_ShaderStage.AddNode(m_TempStorage->Alloc<ast::EndOfExpressionNode>());
            return true;
        }
        else if (ast::OperatorUtil::IsOperator(token.value))
        {
            return ProcessOperator(token, outError);
        }
        else
        {
            outError = {token.line, token.pos, std::format("Parser::ProcessSyntax - unhandled token {}", token.value)};
            return false;
        }
    }

    bool Parser::ProcessPortDeclaration(const Token&, ParseError& outError)
    {
        if (!ExpectAndConsume({TokenType::Syntax}, {"("}, outError))
        {
            return false;
        }

        Token portName;
        if (!ExpectedGetAndConsume({TokenType::Identifier}, {}, portName, outError))
        {
            return false;
        }

        if (!ExpectAndConsume({TokenType::Syntax}, {")"}, outError))
        {
            return false;
        }

        Token direction;
        if (!ExpectedGetAndConsume({TokenType::Builtin}, {"in", "out"}, direction, outError))
        {
            return false;
        }
        bool in = direction.value == "in";

        Token typeToken;
        if (!ExpectedGetAndConsume({TokenType::Builtin}, {"float", "vec2", "vec3", "vec4"}, typeToken, outError))
        {
            return false;
        }
        ast::Type type = ast::TypeUtil::StringToType(typeToken.value);

        Token nameToken;
        if (!ExpectedGetAndConsume({TokenType::Identifier}, {}, nameToken, outError))
        {
            return false;
        }
        std::string name = nameToken.value;

        if (!ExpectAndConsume({TokenType::Syntax}, {";"}, outError))
        {
            return false;
        }

        if (in)
        {
            m_ShaderStage.AddInputPort(m_TempStorage->Alloc<ast::InputPortNode>(portName.value, type, name));
        }
        else
        {
            m_ShaderStage.AddOutputPort(m_TempStorage->Alloc<ast::OutputPortNode>(portName.value, type, name));
        }

        return true;
    }

    bool Parser::ProcessUniformDeclaration(const Token&, ParseError& outError)
    {
        Token typeToken;
        if (!ExpectedGetAndConsume({TokenType::Builtin}, {"float", "vec2", "vec3", "vec4", "mat3", "mat4"}, typeToken,
                                   outError))
        {
            return false;
        }
        ast::Type type = ast::TypeUtil::StringToType(typeToken.value);

        Token nameToken;
        if (!ExpectedGetAndConsume({TokenType::Identifier}, {}, nameToken, outError))
        {
            return false;
        }
        std::string name = nameToken.value;

        if (!ExpectAndConsume({TokenType::Syntax}, {";"}, outError))
        {
            return false;
        }

        std::string temp;
        if (!m_ShaderStage.AddUniform(name, type, temp))
        {
            outError = {nameToken.line, nameToken.pos, temp};
            return false;
        }

        return true;
    }

    bool Parser::ProcessAssignment(const Token& token, ast::Type expectedType, ParseError& outError)
    {
        m_ShaderStage.AddNode(m_TempStorage->Alloc<ast::AssignmentNode>());

        Token nextToken;
        if (!ExpectedGetAndConsume({
                                       TokenType::Identifier, TokenType::Builtin, TokenType::StringLiteral,
                                       TokenType::NumericLiteral
                                   }, {}, nextToken, outError))
        {
            return false;
        }

        switch (nextToken.type)
        {
        case TokenType::Builtin:
            if (ast::TypeUtil::StringToType(nextToken.value) != expectedType)
            {
                outError = {
                    nextToken.line, nextToken.pos,
                    std::format("Cannot assign {} to variable {} of type {}", nextToken.value, token.value,
                                ast::TypeUtil::GetTypeGlsl(expectedType))
                };
                return false;
            }
            if (!ProcessBuiltin(nextToken, outError))
            {
                return false;
            }

            break;
        case TokenType::Identifier:
            ast::Type nextType;
            if (!m_ShaderStage.FindVariable(nextToken.value, nextType))
            {
                outError = {nextToken.line, nextToken.pos, std::format("Undefined variable: {}", nextToken.value)};
                return false;
            }
            m_ShaderStage.AddNode(m_TempStorage->Alloc<ast::VariableReferenceNode>(nextToken.value, m_ShaderStage));
            break;
        case TokenType::NumericLiteral:
            if (IsInteger(nextToken.value))
            {
                m_ShaderStage.AddNode(m_TempStorage->Alloc<ast::ConstantNode<int>>(std::stoi(nextToken.value)));
            }
            else
            {
                m_ShaderStage.AddNode(m_TempStorage->Alloc<ast::ConstantNode<float>>(std::stof(nextToken.value)));
            }
            break;
        case TokenType::StringLiteral:
            m_ShaderStage.AddNode(m_TempStorage->Alloc<ast::ConstantNode<std::string>>(nextToken.value));
            break;
        default:
            SPARK_ASSERT(false);
            break;
        }

        return true;
    }

    bool Parser::ProcessOperator(const Token& token, ParseError&)
    {
        // TODO type validation
        m_ShaderStage.AddNode(m_TempStorage->Alloc<ast::OperatorNode>(ast::OperatorUtil::StringToOperatorType(token.value)));
        return true;
    }

    bool Parser::ProcessFunctionDeclaration(const Token& token, ParseError& outError)
    {
        Token nameToken;
        if (!ExpectedGetAndConsume({TokenType::Identifier}, {}, nameToken, outError))
        {
            return false;
        }

        bool isMain = nameToken.value == "main";
        if (isMain)
        {
            if (m_ShaderStage.IsMainDeclared())
            {
                outError = {nameToken.line, nameToken.pos, "encountered second declaration of main!"};
                return false;
            }
            else if (m_ShaderStage.ScopeDepth() > 0)
            {
                outError = {nameToken.line, nameToken.pos, "main must be declared in the root scope of the shader"};
                return false;
            }
        }

        if (!ExpectAndConsume({TokenType::Syntax}, {"("}, outError))
        {
            return false;
        }

        auto peek = m_Lexer.PeekToken();
        if (std::holds_alternative<Token>(peek))
        {
            auto nextToken = std::get<Token>(peek);
            if (nextToken.type == TokenType::Syntax && nextToken.value == ")")
            {
                //no args
                m_Lexer.ConsumeToken(); // )
                if (!ProcessEndOfFunctionDeclaration(nextToken, outError))
                {
                    return false;
                }

                if (isMain)
                {
                    auto main = m_TempStorage->Alloc<ast::MainNode>();
                    m_ShaderStage.AddNode(main);
                    m_ShaderStage.PushScope(main);
                }
                else
                {
                    //TODO
                    outError = {nameToken.line, nameToken.pos, "Function declarations not supported yet."};
                    return false;
                }
            }
            else if (nextToken.type == TokenType::Builtin)
            {
                if (isMain)
                {
                    outError = {nextToken.line, nextToken.pos, "function arguments not supported for main"};
                    return false;
                }

                //TODO
                outError = {nameToken.line, nameToken.pos, "Function declarations not supported yet."};
                return false;
            }
            else
            {
                outError = {nextToken.line, nextToken.pos, std::format("Unexpected Token: {}", nextToken.value)};
                return false;
            }
        }
        else
        {
            outError = {nameToken.line, nameToken.pos, std::get<std::string>(peek)};
            return false;
        }

        return true;
    }

    bool Parser::ProcessVariableDeclaration(const Token& token, ParseError& outError)
    {
        Token nameToken;
        if (!ExpectedGetAndConsume({TokenType::Identifier}, {}, nameToken, outError))
        {
            return false;
        }

        auto peek = m_Lexer.PeekToken();
        if (std::holds_alternative<Token>(peek))
        {
            auto nextToken = std::get<Token>(peek);
            if (nextToken.type != TokenType::Syntax)
            {
                outError = {nextToken.line, nextToken.pos, std::format("Unexpected Token {0}", nextToken.value)};
            }

            if (nextToken.value == "=")
            {
                m_Lexer.ConsumeToken();
                //cheat and split dec assignment since its still valid code
                ast::Type type = ast::TypeUtil::StringToType(token.value);
                m_ShaderStage.AddNode(m_TempStorage->Alloc<ast::VariableDeclarationNode>(nameToken.value, type));

                std::string error;
                if (!m_ShaderStage.RecordVariableForScope(nameToken.value, type, error))
                {
                    outError = {nameToken.line, nameToken.pos, error};
                    return false;
                }

                if (!ProcessAssignment(nextToken, type, outError))
                {
                    return false;
                }

                if (!ExpectedGetAndConsume({TokenType::Syntax}, {";"}, nextToken, outError))
                {
                    return false;
                }

                m_ShaderStage.AddNode(m_TempStorage->Alloc<ast::EndOfExpressionNode>());
            }
            else if (nextToken.value == ";")
            {
                // we are done;
                m_Lexer.ConsumeToken();
                m_ShaderStage.AddNode(
                    m_TempStorage->Alloc<ast::VariableDeclarationNode>(nameToken.value,
                                                                       ast::TypeUtil::StringToType(token.value)));
                m_ShaderStage.AddNode(m_TempStorage->Alloc<ast::EndOfExpressionNode>());
            }
        }

        return true;
    }

    bool Parser::ProcessEndOfFunctionDeclaration(const Token&, ParseError& outError)
    {
        if (!ExpectAndConsume({TokenType::Syntax}, {"{"}, outError))
        {
            return false;
        }

        return true;
    }

    bool Parser::Expect(const std::vector<TokenType>& allowedTypes, const std::vector<std::string>& allowedValues,
                        ParseError& outError)
    {
        auto peek = m_Lexer.PeekToken();
        if (!std::holds_alternative<Token>(peek))
        {
            outError = {0, 0, "Unexpected End of File!"};
            return false;
        }

        auto token = std::get<Token>(peek);
        bool result = std::ranges::contains(allowedTypes, token.type) &&
            (allowedValues.empty() || std::ranges::contains(allowedValues, token.value));
        if (!result)
        {
            outError = {token.line, token.pos, std::format("Unexpected token: {}", token.value)};
        }

        return result;
    }

    bool Parser::ExpectedGetAndConsume(const std::vector<TokenType>& allowedTypes,
                                       const std::vector<std::string>& allowedValues, Token& token,
                                       ParseError& outError)
    {
        if (!Expect(allowedTypes, allowedValues, outError))
        {
            return false;
        }
        token = std::get<Token>(m_Lexer.PeekToken());
        m_Lexer.ConsumeToken();
        return true;
    }

    bool Parser::ExpectAndConsume(const std::vector<TokenType>& allowedTypes,
                                  const std::vector<std::string>& allowedValues, ParseError& outError)
    {
        if (!Expect(allowedTypes, allowedValues, outError))
        {
            return false;
        }

        m_Lexer.ConsumeToken();
        return true;
    }

    bool Parser::IsInteger(const std::string& value)
    {
        for (char i : value)
        {
            if (!std::isdigit(i))
            {
                return false;
            }
        }

        return true;
    }

    bool Parser::EnsureInteger(const std::string& value, ParseError& outError)
    {
        if (!IsInteger(value))
        {
            outError = {0, 0, std::format("Unexpected character {} while parsing integral value.", value)};
            return false;
        }

        return true;
    }

    bool Parser::ProcessArgument(const Token&, ast::Type& outType, ParseError& outError)
    {
        Token nextToken;
        if (!ExpectedGetAndConsume(
            {TokenType::NumericLiteral, TokenType::Identifier, TokenType::NumericLiteral, TokenType::StringLiteral },
            {}, nextToken, outError))
        {
            return false;
        }

        if (nextToken.type == TokenType::Identifier)
        {
            ast::Type type;
            if (!m_ShaderStage.FindVariable(nextToken.value, type))
            {
                outError = {nextToken.line, nextToken.pos, std::format("Undefined variable {}", nextToken.value) };
                return false;
            }

            m_ShaderStage.AddNode(m_TempStorage->Alloc<ast::VariableReferenceNode>(nextToken.value, m_ShaderStage));
            outType = type;
        }
        else if (nextToken.type == TokenType::NumericLiteral)
        {
            if (!ProcessNumericLiteral(nextToken, outError))
            {
                return false;
            }
            outType = ast::Type::Float; // TODO int?
        }
        else if (nextToken.type == TokenType::StringLiteral)
        {
            if (!ProcessStringLiteral(nextToken, outError))
            {
                return false;
            }
            outType = ast::Type::Invalid; //TODO
        }
        else if (nextToken.type == TokenType::Builtin)
        {
            if (!ProcessBuiltin(nextToken, outError))
            {
                return false;
            }
            outType = ast::TypeUtil::StringToType(nextToken.value);
        }
        else
        {
            outError = {nextToken.line, nextToken.pos, std::format("Unexpected token {}", nextToken.value) };
            return false;
        }

        return true;
    }
}
