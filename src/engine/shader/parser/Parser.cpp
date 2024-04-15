#include "Parser.h"

#include <algorithm>
#include <ranges>
#include <utility>

#include "engine/logging/Log.h"
#include "engine/math/Vec2.h"
#include "engine/math/Vec3.h"
#include "engine/math/Vec4.h"
#include "engine/shader/ast/AssignmentNode.h"
#include "engine/shader/ast/ConstantNode.h"
#include "engine/shader/ast/EndOfExpressionNode.h"
#include "engine/shader/ast/InputAttributeNode.h"
#include "engine/shader/ast/MainNode.h"
#include "engine/shader/ast/OutputNode.h"
#include "engine/shader/ast/PropertyAccessNode.h"
#include "engine/shader/ast/Types.h"
#include "engine/shader/ast/TypeUtil.h"
#include "engine/shader/ast/VariableReferenceNode.h"
#include "engine/shader/ast/VertexPositionOutputNode.h"

namespace se::shader::parser
{
    Parser::Parser(Lexer lexer)
        : m_Lexer(std::move(lexer))
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
                        if (!ProcessTopLevelNumericLiteral(token, error))
                        {
                            return error;
                        }
                        break;
                    case TokenType::StringLiteral:
                        if (!ProcessTopLevelStringLiteral(token, error))
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
                logging::Log::Error(std::get<std::string>(nextToken).c_str());
            }
        }

        return m_ShaderStage;
    }

    bool Parser::ProcessVec2(const Token &token, ParseError& outError)
    {
        Token nextToken;
        if (!ExpectedGetAndConsume({ TokenType::Syntax }, { "(" }, nextToken, outError))
        {
            return false;
        }

        if (!ExpectedGetAndConsume({ TokenType::NumericLiteral }, { }, nextToken, outError))
        {
            return false;
        }
        float x = std::stof(nextToken.value);

        if (!ExpectedGetAndConsume({ TokenType::Syntax }, { "," }, nextToken, outError))
        {
            return false;
        }

        if (!ExpectedGetAndConsume({ TokenType::NumericLiteral }, { }, nextToken, outError))
        {
            return false;
        }
        float y = std::stof(nextToken.value);

        if (!ExpectedGetAndConsume({ TokenType::Syntax }, { ")" }, nextToken, outError))
        {
            return false;
        }

        m_ShaderStage.AddNode(m_TempStorage.Alloc<ast::ConstantNode<math::Vec2>>(math::Vec2(x, y)));

        return true;
    }

    bool Parser::ProcessVec3(const Token &token, ParseError& outError)
    {
        Token nextToken;
        if (!ExpectedGetAndConsume({ TokenType::Syntax }, { "(" }, nextToken, outError))
        {
            return false;
        }

        if (!ExpectedGetAndConsume({ TokenType::NumericLiteral }, { }, nextToken, outError))
        {
            return false;
        }
        float x = std::stof(nextToken.value);

        if (!ExpectedGetAndConsume({ TokenType::Syntax }, { "," }, nextToken, outError))
        {
            return false;
        }

        if (!ExpectedGetAndConsume({ TokenType::NumericLiteral }, { }, nextToken, outError))
        {
            return false;
        }
        float y = std::stof(nextToken.value);

        if (!ExpectedGetAndConsume({ TokenType::Syntax }, { "," }, nextToken, outError))
        {
            return false;
        }

        if (!ExpectedGetAndConsume({ TokenType::NumericLiteral }, { }, nextToken, outError))
        {
            return false;
        }
        float z = std::stof(nextToken.value);

        if (!ExpectedGetAndConsume({ TokenType::Syntax }, { ")" }, nextToken, outError))
        {
            return false;
        }

        m_ShaderStage.AddNode(m_TempStorage.Alloc<ast::ConstantNode<math::Vec3>>(math::Vec3(x, y, z)));

        return true;
    }

    bool Parser::ProcessVec4(const Token &token, ParseError& outError)
    {
        Token nextToken;
        if (!ExpectedGetAndConsume({ TokenType::Syntax }, { "(" }, nextToken, outError))
        {
            return false;
        }

        if (!ExpectedGetAndConsume({ TokenType::NumericLiteral }, { }, nextToken, outError))
        {
            return false;
        }
        float x = std::stof(nextToken.value);

        if (!ExpectedGetAndConsume({ TokenType::Syntax }, { "," }, nextToken, outError))
        {
            return false;
        }

        if (!ExpectedGetAndConsume({ TokenType::NumericLiteral }, { }, nextToken, outError))
        {
            return false;
        }
        float y = std::stof(nextToken.value);

        if (!ExpectedGetAndConsume({ TokenType::Syntax }, { "," }, nextToken, outError))
        {
            return false;
        }

        if (!ExpectedGetAndConsume({ TokenType::NumericLiteral }, { }, nextToken, outError))
        {
            return false;
        }
        float z = std::stof(nextToken.value);

        if (!ExpectedGetAndConsume({ TokenType::Syntax }, { "," }, nextToken, outError))
        {
            return false;
        }

        if (!ExpectedGetAndConsume({ TokenType::NumericLiteral }, { }, nextToken, outError))
        {
            return false;
        }
        float w = std::stof(nextToken.value);

        if (!ExpectedGetAndConsume({ TokenType::Syntax }, { ")" }, nextToken, outError))
        {
            return false;
        }

        m_ShaderStage.AddNode(m_TempStorage.Alloc<ast::ConstantNode<math::Vec4>>(math::Vec4(x, y, z, w)));

        return true;
    }

    bool Parser::ProcessBuiltin(const Token &token, ParseError& outError)
    {
        if (token.value == "layout")
        {
            return ProcessLayoutDeclaration(token, outError);
        }
        else if (token.value == "void")
        {
            return ProcessFunctionDeclaration(token, outError);
        }
        else if (token.value == "gl_Position")
        {
            return ProcessGLPositionWrite(token, outError);
        }
        else if (token.value == "out")
        {
            return ProcessOutputDeclaration(token, outError);
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

        outError = { token.line, token.pos, std::format("Unexpected token {}", token.value) };
        SPARK_ASSERT(false);
        return false;
    }

    bool Parser::ProcessIdentifier(const Token &token, ParseError& outError)
    {
        ast::Type type;
        if (!m_ShaderStage.FindVariable(token.value, type))
        {
            outError = { token.line, token.pos, std::format("Undefined variable: {}", token.value) };
            return false;
        }

        m_ShaderStage.AddNode(m_TempStorage.Alloc<ast::VariableReferenceNode>(token.value, m_ShaderStage));

        Token nextToken;
        if (!ExpectedGetAndConsume({TokenType::Syntax}, {"=", "."}, nextToken, outError))
        {
            return false;
        }

        if (nextToken.value == "=")
        {
            m_ShaderStage.AddNode(m_TempStorage.Alloc<ast::AssignmentNode>());

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
                    if (ast::TypeUtil::StringToType(nextToken.value) != type)
                    {
                        outError = {nextToken.line, nextToken.pos, std::format("Cannot assign {} to variable {} of type {}", nextToken.value, token.value, ast::TypeUtil::GetTypeGlsl(type)) };
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
                        outError = { nextToken.line, nextToken.pos, std::format("Undefined variable: {}", nextToken.value) };
                        return false;
                    }
                    m_ShaderStage.AddNode(m_TempStorage.Alloc<ast::VariableReferenceNode>(nextToken.value, m_ShaderStage));
                    break;
                case TokenType::NumericLiteral:
                    if (IsInteger(nextToken.value))
                    {
                        m_ShaderStage.AddNode(m_TempStorage.Alloc<ast::ConstantNode<int>>(std::stoi(nextToken.value)));
                    }
                    else
                    {
                        m_ShaderStage.AddNode(m_TempStorage.Alloc<ast::ConstantNode<float>>(std::stof(nextToken.value)));
                    }
                    break;
                case TokenType::StringLiteral:
                    m_ShaderStage.AddNode(m_TempStorage.Alloc<ast::ConstantNode<std::string>>(nextToken.value));
                    break;
                default:
                    SPARK_ASSERT(false);
                    break;
            }

            if (!ExpectedGetAndConsume({TokenType::Syntax}, {";"}, nextToken, outError))
            {
                return false;
            }

            m_ShaderStage.AddNode(m_TempStorage.Alloc<ast::EndOfExpressionNode>());
            return true;
        }

        outError = { nextToken.line, nextToken.pos, std::format("Parser::ProcessIdentifier - Not Implemented: {}", nextToken.value) };
        return false;
    }

    bool Parser::ProcessTopLevelNumericLiteral(const Token &token, ParseError& outError)
    {
        outError = { token.line, token.pos, std::format("Parser::ProcessTopLevelNumericLiteral - Not Implemented: {}", token.value) };
        return false;
    }

    bool Parser::ProcessTopLevelStringLiteral(const Token &token, ParseError& outError)
    {
        outError = { token.line, token.pos, std::format("Parser::ProcessTopLevelStringLiteral - Not Implemented: {}", token.value) };
        return false;
    }

    bool Parser::ProcessSyntax(const Token &token, ParseError& outError)
    {
        if (token.value == "}")
        {
            m_ShaderStage.PopScope();
            return true;
        }
        else
        {
            outError = { token.line, token.pos, std::format("Parser::ProcessSyntax - unhandled token {}", token.value) };
            return false;
        }
    }

    bool Parser::ProcessLayoutDeclaration(const Token &token, ParseError& outError)
    {
        if (!ExpectAndConsume({TokenType::Syntax}, {"("}, outError))
        {
            return false;
        }

        if (!ExpectAndConsume({TokenType::Identifier}, {"location"}, outError))
        {
            return false;
        }

        if (!ExpectAndConsume({TokenType::Syntax}, {"="}, outError))
        {
            return false;
        }

        Token numericLiteral;
        if (!ExpectedGetAndConsume({TokenType::NumericLiteral}, {}, numericLiteral, outError))
        {
            return false;
        }
        if (!EnsureInteger(numericLiteral.value, outError))
        {
            return false;
        }

        int location = std::stoi(numericLiteral.value);
        if (location < 0)
        {
            outError = { numericLiteral.line, numericLiteral.pos, "Invalid: layout location must be a positive integer value." };
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
            m_ShaderStage.AddInput(m_TempStorage.Alloc<ast::InputAttributeNode>(location, type, name));
        }
        else
        {
            m_ShaderStage.AddOutput(m_TempStorage.Alloc<ast::OutputNode>(type, name));
        }

        return true;
    }

    bool Parser::ProcessOutputDeclaration(const Token &token, ParseError& outError)
    {
        Token varType;
        if (!ExpectedGetAndConsume({TokenType::Builtin}, {}, varType, outError))
        {
            return false;
        }

        ast::Type type = ast::TypeUtil::StringToType(varType.value);
        if (type == ast::Type::Invalid)
        {
            outError = { varType.line, varType.pos, std::format("Invalid type {} in output declaration", varType.value) };
            return false;
        }

        Token nameToken;
        if (!ExpectedGetAndConsume({TokenType::Identifier}, {}, nameToken, outError))
        {
            return false;
        }

        if (!ExpectAndConsume({TokenType::Syntax}, {";"}, outError))
        {
            return false;
        }

        m_ShaderStage.AddOutput(m_TempStorage.Alloc<ast::OutputNode>(type, nameToken.value));
        return true;
    }

    bool Parser::ProcessFunctionDeclaration(const Token &token, ParseError& outError)
    {
        ast::Type type = ast::TypeUtil::StringToType(token.value);

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
                outError = { nameToken.line, nameToken.pos, "encountered second declaration of main!" };
                return false;
            }
            else if (m_ShaderStage.ScopeDepth() > 0)
            {
                outError = { nameToken.line, nameToken.pos, "main must be declared in the root scope of the shader" };
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
                    auto main = m_TempStorage.Alloc<ast::MainNode>();
                    m_ShaderStage.AddNode(main);
                    m_ShaderStage.PushScope(main);
                }
                else
                {
                    //TODO
                    outError = { nameToken.line, nameToken.pos, "Function declarations not supported yet." };
                    return false;
                }
            }
            else if (nextToken.type == TokenType::Builtin)
            {
                if (isMain)
                {
                    outError = { nextToken.line, nextToken.pos, "function arguments not supported for main" };
                    return false;
                }

                if (!ProcessFunctionArguments(nextToken, outError))
                {
                    return false;
                }

                m_Lexer.ConsumeToken(); // )
                if (!ProcessEndOfFunctionDeclaration(nextToken, outError))
                {
                    return false;
                }

                //TODO
                outError = { nameToken.line, nameToken.pos, "Function declarations not supported yet." };
                return false;
            }
            else
            {
                outError = { nextToken.line, nextToken.pos, std::format("Unexpected Token: {}", nextToken.value) };
                return false;
            }
        }
        else
        {
            outError = { nameToken.line, nameToken.pos, std::get<std::string>(peek) };
            return false;
        }

        return true;
    }

    bool Parser::ProcessFunctionArguments(const Token &token, ParseError& outError)
    {
        //TODO
        outError = { token.line, token.pos, "Function arguments not supported yet." };
        return false;
    }

    bool Parser::ProcessEndOfFunctionDeclaration(const Token &token, ParseError& outError)
    {
        if (!ExpectAndConsume({TokenType::Syntax}, {"{"}, outError))
        {
            return false;
        }

        return true;
    }

    bool Parser::ProcessGLPositionWrite(const Token &token, ParseError& outError)
    {
        Token op;
        if (!ExpectedGetAndConsume({TokenType::Syntax}, {".", "="}, op, outError))
        {
            return false;
        }

        m_ShaderStage.AddNode(m_TempStorage.Alloc<ast::VertexPositionOutputNode>());
        ast::Type allowedWriteType = ast::Type::Vec4;
        if (op.value == ".")
        {
            Token propertyName;
            if (!ExpectedGetAndConsume({TokenType::Identifier}, {}, propertyName, outError))
            // TODO validate property access based on type
            {
                return false;
            }

            m_ShaderStage.AddNode(m_TempStorage.Alloc<ast::PropertyAccessNode>(propertyName.value));
            if (propertyName.value == "xyz")
            {
                allowedWriteType = ast::Type::Vec3;
            }
            else if (propertyName.value == "x")
            {
                allowedWriteType = ast::Type::Float;
            }
            else if (propertyName.value == "y")
            {
                allowedWriteType = ast::Type::Float;
            }
            else if (propertyName.value == "z")
            {
                allowedWriteType = ast::Type::Float;
            }
            else if (propertyName.value == "w")
            {
                allowedWriteType = ast::Type::Float;
            }
            else
            {
                outError = { propertyName.line, propertyName.pos, std::format("Unkown gl_Position property: {}", propertyName.value) };
            }

            if (!ExpectedGetAndConsume({TokenType::Syntax}, {"="}, op, outError))
            {
                return false;
            }
        }

        if (op.value != "=")
        {
            outError = { op.line, op.pos, std::format("Parser::ProcessGLPositionWrite - Expected =, got {}", op.value) };
            return false;
        }

        m_ShaderStage.AddNode(m_TempStorage.Alloc<ast::AssignmentNode>());

        Token assignmentValue;
        if (!ExpectedGetAndConsume({TokenType::Identifier, TokenType::Builtin, TokenType::NumericLiteral}, {},
                                   assignmentValue, outError))
        {
            return false;
        }

        switch (assignmentValue.type)
        {
            case TokenType::Identifier:
            {
                m_ShaderStage.AddNode(m_TempStorage.Alloc<ast::VariableReferenceNode>(assignmentValue.value, m_ShaderStage));
                if (!ExpectAndConsume({TokenType::Syntax}, {";"}, outError))
                {
                    return false;
                }

                m_ShaderStage.AddNode(m_TempStorage.Alloc<ast::EndOfExpressionNode>());
                break;
            }
            case TokenType::NumericLiteral:
            case TokenType::Builtin:
            {
                if (assignmentValue.type != TokenType::NumericLiteral)
                {
                    if (ast::TypeUtil::StringToType(assignmentValue.value) != allowedWriteType)
                    {
                        outError = { assignmentValue.line, assignmentValue.pos, std::format("Unexpected Type: {}", assignmentValue.value) };
                        return false;
                    }
                }

                switch (allowedWriteType)
                {
                    case ast::Type::Float:
                    {
                        if (assignmentValue.type != TokenType::NumericLiteral)
                        {
                            outError = { assignmentValue.line, assignmentValue.pos, std::format("Unexpected Token: {}", assignmentValue.value) };
                            return false;
                        }
                        m_ShaderStage.AddNode(m_TempStorage.Alloc<ast::ConstantNode<float>>(std::stof(assignmentValue.value)));

                        if (!ExpectAndConsume({TokenType::Syntax}, {";"}, outError))
                        {
                            return false;
                        }

                        m_ShaderStage.AddNode(m_TempStorage.Alloc<ast::EndOfExpressionNode>());

                        break;
                    }
                    case ast::Type::Vec3:
                        //TODO
                        return false;
                    case ast::Type::Vec4:
                        //TODO
                        return false;
                }
                break;
            }
            default:
                outError = { assignmentValue.line, assignmentValue.pos, std::format("Unexpected Type: {}", assignmentValue.value) };
                return false;
        }

        return true;
    }

    bool Parser::Expect(const std::vector<TokenType> &allowedTypes, const std::vector<std::string> &allowedValues,
                        ParseError& outError)
    {
        auto peek = m_Lexer.PeekToken();
        if (!std::holds_alternative<Token>(peek))
        {
            outError = { 0, 0, "Unexpected End of File!" };
            return false;
        }

        auto token = std::get<Token>(peek);
        bool result = std::ranges::contains(allowedTypes, token.type) &&
                      (allowedValues.empty() || std::ranges::contains(allowedValues, token.value));
        if (!result)
        {
            outError = { token.line, token.pos, std::format("Unexpected token: {}", token.value) };
        }

        return result;
    }

    bool Parser::ExpectedGetAndConsume(const std::vector<TokenType> &allowedTypes,
                                       const std::vector<std::string> &allowedValues, Token &token,
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

    bool Parser::ExpectAndConsume(const std::vector<TokenType> &allowedTypes,
                                  const std::vector<std::string> &allowedValues, ParseError& outError)
    {
        if (!Expect(allowedTypes, allowedValues, outError))
        {
            return false;
        }

        m_Lexer.ConsumeToken();
        return true;
    }

    bool Parser::IsInteger(const std::string &value)
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

    bool Parser::EnsureInteger(const std::string &value, ParseError& outError)
    {
        if (!IsInteger(value))
        {
            outError = { 0, 0, std::format("Unexpected character {} while parsing integral value.", value) };
            return false;
        }

        return true;
    }
}
