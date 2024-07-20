#include "Parser.h"

#include "engine/asset/shader/ast/TextureSampleNode.h"
#include "engine/asset/shader/ast/PropertyAccessNode.h"
#include "engine/asset/shader/ast/AnonymousScopeNode.h"
#include "engine/asset/shader/ast/BinaryExpressionNode.h"
#include "engine/asset/shader/ast/ClampNode.h"
#include "engine/asset/shader/ast/ConstantNode.h"
#include "engine/asset/shader/ast/DotNode.h"
#include "engine/asset/shader/ast/EndOfExpressionNode.h"
#include "engine/asset/shader/ast/InputPortNode.h"
#include "engine/asset/shader/ast/LengthNode.h"
#include "engine/asset/shader/ast/MainNode.h"
#include "engine/asset/shader/ast/NormalizeNode.h"
#include "engine/asset/shader/ast/Operators.h"
#include "engine/asset/shader/ast/OutputPortNode.h"
#include "engine/asset/shader/ast/PowNode.h"
#include "engine/asset/shader/ast/ReflectNode.h"
#include "engine/asset/shader/ast/Types.h"
#include "engine/asset/shader/ast/TypeUtil.h"
#include "engine/asset/shader/ast/VariableDeclarationNode.h"
#include "engine/asset/shader/ast/VariableReferenceNode.h"
#include "engine/asset/shader/ast/Vec2Node.h"
#include "engine/asset/shader/ast/Vec3Node.h"
#include "engine/asset/shader/ast/Vec4Node.h"

namespace se::asset::shader::compiler
{
    Parser::Parser(Lexer lexer)
        : m_Lexer(std::move(lexer))
    {
    }

    std::variant<ast::Shader, ParseError> Parser::Parse()
    {
        ParseError error;
        while (!m_Lexer.Finished())
        {
            auto nextToken = m_Lexer.PeekToken();
            if (std::holds_alternative<Token>(nextToken))
            {
                Token token = std::get<Token>(nextToken);

                switch (token.type)
                {
                case TokenType::Builtin:
                    m_Lexer.ConsumeToken();
                    ast::Type builtinType;
                    if (!ProcessBuiltin(token, builtinType, error))
                    {
                        return error;
                    }
                    break;
                case TokenType::Identifier:
                    ast::Type type;
                    if (!ProcessExpression(type, error))
                    {
                        return error;
                    }
                    break;
                case TokenType::NumericLiteral:
                    m_Lexer.ConsumeToken();
                    if (!ProcessNumericLiteral(token, error))
                    {
                        return error;
                    }
                    break;
                case TokenType::StringLiteral:
                    m_Lexer.ConsumeToken();
                    if (!ProcessStringLiteral(token, error))
                    {
                        return error;
                    }
                    break;
                case TokenType::Syntax:
                    m_Lexer.ConsumeToken();
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

        return m_Shader;
    }

    bool Parser::ProcessVec2(const Token&, ParseError& outError)
    {
        Token nextToken;
        if (!ExpectedGetAndConsume({TokenType::Syntax}, {"("}, nextToken, outError))
        {
            return false;
        }

        auto vec4 = std::make_shared<ast::Vec2Node>();
        m_Shader.AddNode(vec4);
        m_Shader.PushScope(vec4);
        int componentsAccountedFor = 0;
        while (componentsAccountedFor < 2)
        {
            ast::Type argType;
            if (!ProcessExpression(argType, outError))
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
            case ast::Type::Sampler2D:
                outError = {
                    nextToken.line, nextToken.pos,
                    std::format("Unexpected type {}", ast::TypeUtil::GetTypeGlsl(argType))
                };
                return false;
                break;
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

        m_Shader.PopScope();

        return true;
    }

    bool Parser::ProcessVec3(const Token&, ParseError& outError)
    {
        Token nextToken;
        if (!ExpectedGetAndConsume({TokenType::Syntax}, {"("}, nextToken, outError))
        {
            return false;
        }

        auto vec4 = std::make_shared<ast::Vec3Node>();
        m_Shader.AddNode(vec4);
        m_Shader.PushScope(vec4);
        int componentsAccountedFor = 0;
        while (componentsAccountedFor < 3)
        {
            ast::Type argType;
            if (!ProcessExpression(argType, outError))
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
                componentsAccountedFor += 3;
                break;
            case ast::Type::Mat4:
                if (!Expect({TokenType::Syntax}, {"*"}, outError))
                {
                    return false;
                }
                break;
            case ast::Type::Mat3:
            case ast::Type::Void:
            case ast::Type::Invalid:
            case ast::Type::Sampler2D:
                outError = {
                    nextToken.line, nextToken.pos,
                    std::format("Unexpected type {}", ast::TypeUtil::GetTypeGlsl(argType))
                };
                return false;
            }

            if (componentsAccountedFor < 3)
            {
                if (!ExpectedGetAndConsume({TokenType::Syntax}, {",", "*", "/"}, nextToken, outError))
                {
                    return false;
                }
            }
        }

        if (!ExpectedGetAndConsume({TokenType::Syntax}, {")"}, nextToken, outError))
        {
            return false;
        }

        m_Shader.PopScope();

        return true;
    }

    bool Parser::ProcessVec4(const Token&, ParseError& outError)
    {
        Token nextToken;
        if (!ExpectedGetAndConsume({TokenType::Syntax}, {"("}, nextToken, outError))
        {
            return false;
        }

        auto vec4 = std::make_shared<ast::Vec4Node>();
        m_Shader.AddNode(vec4);
        m_Shader.PushScope(vec4);
        int componentsAccountedFor = 0;
        while (componentsAccountedFor < 4)
        {
            ast::Type argType;
            if (!ProcessExpression(argType, outError))
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
            case ast::Type::Sampler2D:
            case ast::Type::Invalid:
                outError = {
                    nextToken.line, nextToken.pos,
                    std::format("Unexpected type {}", ast::TypeUtil::GetTypeGlsl(argType))
                };
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

        m_Shader.PopScope();

        return true;
    }

    bool Parser::ProcessBuiltin(const Token& token, ast::Type& returnType, ParseError& outError)
    {
        returnType = ast::Type::Invalid;

        bool isPossibleVariableDec = ast::TypeUtil::StringToType(token.value) != ast::Type::Invalid;
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
            returnType = ast::Type::Void;
            return ProcessFunctionDeclaration(token, outError);
        }
        else if (token.value == "vec2")
        {
            returnType = ast::Type::Vec2;
            return ProcessVec2(token, outError);
        }
        else if (token.value == "vec3")
        {
            returnType = ast::Type::Vec3;
            return ProcessVec3(token, outError);
        }
        else if (token.value == "vec4")
        {
            returnType = ast::Type::Vec4;
            return ProcessVec4(token, outError);
        }
        else if (token.value == "port")
        {
            return ProcessPortDeclaration(token, returnType, outError);
        }
        else if (token.value == "uniform")
        {
            return ProcessUniformDeclaration(token, returnType, outError);
        }
        else if (token.value == "texture")
        {
            returnType = ast::Type::Vec4;
            return ProcessTextureRead(token, outError);
        }
        else if (token.value == "length")
        {
            returnType = ast::Type::Float;
            return ProcessLengthFunc(token, outError);
        }
        else if (token.value == "normalize")
        {
            return ProcessNormalizeFunc(token, returnType, outError);
        }
        else if (token.value == "clamp")
        {
            returnType = ast::Type::Float;
            return ProcessClampFunc(token, outError);
        }
        else if (token.value == "dot")
        {
            returnType = ast::Type::Float;
            return ProcessDotFunc(token, outError);
        }
        else if (token.value == "reflect")
        {
            return ProcessReflectFunc(token, returnType, outError);
        }
        else if (token.value == "pow")
        {
            returnType = ast::Type::Float;
            return ProcessPowFunc(token, outError);
        }

        outError = {token.line, token.pos, std::format("Unexpected token {}", token.value)};
        SPARK_ASSERT(false);
        return false;
    }

    bool Parser::ProcessNumericLiteral(const Token& token, ParseError&)
    {
        if (IsInteger(token.value))
        {
            m_Shader.AddNode(std::make_shared<ast::ConstantNode<int>>(std::stoi(token.value)));
        }
        else
        {
            m_Shader.AddNode(
                std::make_shared<ast::ConstantNode<float>>(std::stof(token.value)));

            auto peek = m_Lexer.PeekToken();
            if (std::holds_alternative<Token>(peek))
            {
                auto token = std::get<Token>(peek);
                if (token.value == "f")
                {
                    m_Lexer.ConsumeToken();
                }
            }
        }
        return true;
    }

    bool Parser::ProcessStringLiteral(const Token& token, ParseError&)
    {
        m_Shader.AddNode(std::make_shared<ast::ConstantNode<std::string>>(token.value));
        return true;
    }

    bool Parser::ProcessSyntax(const Token& token, ParseError& outError)
    {
        if (token.value == "}")
        {
            m_Shader.PopScope();
            return true;
        }
        else if (token.value == "{")
        {
            auto scope = std::make_shared<ast::AnonymousScopeNode>();
            m_Shader.AddNode(scope);
            m_Shader.PushScope(scope);
            return true;
        }
        else if (token.value == ";")
        {
            m_Shader.AddNode(std::make_shared<ast::EndOfExpressionNode>());
            return true;
        }
        else
        {
            outError = {token.line, token.pos, std::format("Parser::ProcessSyntax - unhandled token {}", token.value)};
            return false;
        }
    }

    bool Parser::ProcessPortDeclaration(const Token&, ast::Type& returnType, ParseError& outError)
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
        returnType = type;

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
            m_Shader.AddInputPort(std::make_shared<ast::InputPortNode>(portName.value, type, name));
        }
        else
        {
            m_Shader.AddOutputPort(std::make_shared<ast::OutputPortNode>(portName.value, type, name));
        }

        return true;
    }

    bool Parser::ProcessUniformDeclaration(const Token&, ast::Type& returnType, ParseError& outError)
    {
        Token typeToken;
        if (!ExpectedGetAndConsume({TokenType::Builtin}, ast::TypeUtil::GetTypeStrings(), typeToken,
                                   outError))
        {
            return false;
        }
        ast::Type type = ast::TypeUtil::StringToType(typeToken.value);
        returnType = type;

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
        if (!m_Shader.AddUniform(name, type, temp))
        {
            outError = {nameToken.line, nameToken.pos, temp};
            return false;
        }

        return true;
    }

    bool Parser::Peek(int offset, const std::vector<TokenType>& allowedTypes,
        const std::vector<std::string>& allowedValues)
    {
        Token token;
        return Peek(offset, allowedTypes, allowedValues, token);
    }

    bool Parser::Peek(const std::vector<TokenType>& allowedTypes, const std::vector<std::string>& allowedValues)
    {
        return Peek(0, allowedTypes, allowedValues);
    }

    bool Parser::Peek(const std::vector<TokenType>& allowedTypes, const std::vector<std::string>& allowedValues, Token& outToken)
    {
        return Peek(0, allowedTypes, allowedValues, outToken);
    }

    bool Parser::Peek(int offset, const std::vector<TokenType>& allowedTypes,
        const std::vector<std::string>& allowedValues, Token& outToken)
    {
        auto peek = m_Lexer.PeekTokenAt(offset);
        if (!std::holds_alternative<Token>(peek))
        {
            return false;
        }

        outToken = std::get<Token>(peek);
        return (allowedTypes.empty() || std::ranges::contains(allowedTypes, outToken.type)) &&
            (allowedValues.empty() || std::ranges::contains(allowedValues, outToken.value));
    }

    bool Parser::IsNextTokenBinaryOperator()
    {
        return Peek({ TokenType::Syntax }, { "*", "/", "-", "+", "*=", "/=", "-=", "+="});
    }

    bool Parser::ProcessFunctionDeclaration(const Token&, ParseError& outError)
    {
        Token nameToken;
        if (!ExpectedGetAndConsume({TokenType::Identifier}, {}, nameToken, outError))
        {
            return false;
        }

        bool isMain = nameToken.value == "main";
        if (isMain)
        {
            if (m_Shader.IsMainDeclared())
            {
                outError = {nameToken.line, nameToken.pos, "encountered second declaration of main!"};
                return false;
            }
            else if (m_Shader.ScopeDepth() > 0)
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
                    auto main = std::make_shared<ast::MainNode>();
                    m_Shader.AddNode(main);
                    m_Shader.PushScope(main);
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
        if (!ExpectedGet({TokenType::Identifier}, {}, nameToken, outError))
        {
            return false;
        }

        ast::Type declarationType = ast::TypeUtil::StringToType(token.value);
        m_Shader.AddNode(std::make_shared<ast::VariableDeclarationNode>(nameToken.value, declarationType));
        m_Shader.AddNode(std::make_shared<ast::EndOfExpressionNode>());

        std::string error;
        if (!m_Shader.RecordVariableForScope(nameToken.value, declarationType, error))
        {
            outError = {nameToken.line, nameToken.pos, error};
            return false;
        }

        ast::Type expressionType;
        if (!ProcessExpression(expressionType, outError))
        {
            return false;
        }

        if (declarationType != expressionType)
        {
            outError = {token.line, token.pos, std::format("Cannot assign value of type {} to {}", token.value, ast::TypeUtil::GetTypeGlsl(expressionType))};
            return false;
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

    bool Parser::ExpectedGet(const std::vector<TokenType>& allowedTypes, const std::vector<std::string>& allowedValues,
        Token& token, ParseError& outError)
    {
        if (!Expect(allowedTypes, allowedValues, outError))
        {
            return false;
        }
        token = std::get<Token>(m_Lexer.PeekToken());
        return true;
    }

    bool Parser::ExpectedGetAndConsume(const std::vector<TokenType>& allowedTypes,
                                       const std::vector<std::string>& allowedValues, Token& token,
                                       ParseError& outError)
    {
        if (!ExpectedGet(allowedTypes, allowedValues, token, outError))
        {
            return false;
        }
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

    bool Parser::ProcessExpression(ast::Type& outType, ParseError& outError)
    {
        int numBinaryExpressions = 0;
        while (true)
        {
            Token binaryOpToken;
            if (Peek({ TokenType::Syntax }, {"*", "/", "+", "-", "*=", "/=", "+=", "-=", "="}, binaryOpToken))
            {
                auto binaryOp = std::make_shared<ast::BinaryExpressionNode>(ast::OperatorUtil::StringToOperatorType(binaryOpToken.value));
                m_Shader.AddNode(binaryOp);
                m_Shader.PushScope(binaryOp);
                numBinaryExpressions++;
                m_Lexer.ConsumeToken();
            }

            Token nextToken;
            if (!ExpectedGetAndConsume(
                {
                    TokenType::NumericLiteral, TokenType::Identifier,
                    TokenType::StringLiteral,
                    TokenType::Builtin
                },
                {}, nextToken, outError))
            {
                return false;
            }

            bool deferBinaryOpConsume = false;
            int binaryOpPeekOffset = 0;

            // handle func or constructor in binary op
            if (Peek({ TokenType::Syntax }, {"("}))
            {
                std::variant<Token, std::string> peekVal = m_Lexer.PeekTokenAt(binaryOpPeekOffset);
                while (std::holds_alternative<Token>(peekVal))
                {
                    peekVal = m_Lexer.PeekTokenAt(binaryOpPeekOffset);
                    binaryOpPeekOffset++;

                    Token peekToken = std::get<Token>(peekVal);
                    if (peekToken.value == ")")
                    {
                        break;
                    }
                }

                if (!std::holds_alternative<Token>(peekVal))
                {
                    outError = {0, 0, "Unexpected End of File!"};
                    return false;
                }
            }

            // handle prop access in binary op
            if (Peek(binaryOpPeekOffset, {TokenType::Syntax}, {"."}))
            {
                binaryOpPeekOffset += 2; // skip propery access
            }

            if (Peek(binaryOpPeekOffset, { TokenType::Syntax }, {"*", "/", "+", "-", "*=", "/=", "+=", "-=", "="}, binaryOpToken))
            {
                auto binaryOp = std::make_shared<ast::BinaryExpressionNode>(ast::OperatorUtil::StringToOperatorType(binaryOpToken.value));
                m_Shader.AddNode(binaryOp);
                m_Shader.PushScope(binaryOp);
                numBinaryExpressions++;
                if (binaryOpPeekOffset == 0)
                {
                    m_Lexer.ConsumeToken();
                }
                else
                {
                    deferBinaryOpConsume = true;
                }
            }

            if (nextToken.type == TokenType::Identifier)
            {
                ast::Type type;
                if (!m_Shader.FindVariable(nextToken.value, type))
                {
                    outError = {nextToken.line, nextToken.pos, std::format("Undefined variable {}", nextToken.value)};
                    return false;
                }

                // check for property access
                bool isPropertyAccess = false;
                auto peek = m_Lexer.PeekToken();
                if (std::holds_alternative<Token>(peek) && std::get<Token>(peek).value == ".")
                {
                    if (!ProcessPropertyAccess(nextToken, outType, outError))
                    {
                        return false;
                    }

                    isPropertyAccess = true;
                }
                else
                {
                    outType = type;
                }

                m_Shader.AddNode(std::make_shared<ast::VariableReferenceNode>(nextToken.value, m_Shader));
                if (isPropertyAccess)
                {
                    m_Shader.PopScope();
                }
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
                ast::Type builtinType;
                if (!ProcessBuiltin(nextToken, builtinType, outError))
                {
                    return false;
                }

                // check for property access
                bool isPropertyAccess = false;
                auto peek = m_Lexer.PeekToken();
                if (std::holds_alternative<Token>(peek) && std::get<Token>(peek).value == ".")
                {
                    if (!ProcessPropertyAccess(nextToken, outType, outError))
                    {
                        return false;
                    }

                    isPropertyAccess = true;
                }
                else
                {
                    outType = builtinType;
                }

                if (isPropertyAccess)
                {
                    // we need to move the builtin to a child of the property access.
                    // This would ideally be refactored to add them in the correct order in the first place.
                    auto currentParentNode = (m_Shader.GetScopeStack().end() - 2)->m_Node;
                    auto propertyAccess = currentParentNode->m_Children.back();
                    auto builtinIt = currentParentNode->m_Children.end() - 2;
                    auto builtin = *builtinIt;
                    currentParentNode->m_Children.erase(builtinIt);
                    propertyAccess->m_Children.push_back(builtin);

                    m_Shader.PopScope();
                }
            }
            else
            {
                outError = {nextToken.line, nextToken.pos, std::format("Unexpected token {}", nextToken.value)};
                return false;
            }

            if (deferBinaryOpConsume)
            {
                if (SPARK_VERIFY(Peek({ TokenType::Syntax }, {"*", "/", "+", "-", "*=", "/=", "+=", "-=", "="})))
                {
                    m_Lexer.ConsumeToken();
                }
            }

            if (Peek({ TokenType::Syntax }, {")", ",", ";"}))
            {
                break;
            }
        }

        for (int i = 0; i < numBinaryExpressions; ++i)
        {
            m_Shader.PopScope();
        }

        return true;
    }

    bool Parser::ProcessTextureRead(const Token&, ParseError& outError)
    {
        if (!ExpectAndConsume({TokenType::Syntax}, {"("}, outError))
        {
            return false;
        }

        Token textureVariableToken;
        if (!ExpectedGetAndConsume({TokenType::Identifier}, {}, textureVariableToken, outError))
        {
            return false;
        }
        ast::Type varType;
        if (!m_Shader.FindVariable(textureVariableToken.value, varType))
        {
            outError = {
                textureVariableToken.line, textureVariableToken.pos,
                std::format("Undefined variable: {}", textureVariableToken.value)
            };
            return false;
        }
        if (varType != ast::Type::Sampler2D)
        {
            outError = {
                textureVariableToken.line, textureVariableToken.pos,
                std::format("Unexpected type: {} Expected: sampler2D", ast::TypeUtil::GetTypeGlsl(varType))
            };
            return false;
        }

        if (!ExpectAndConsume({TokenType::Syntax}, {","}, outError))
        {
            return false;
        }

        Token uvVariableToken;
        if (!ExpectedGetAndConsume({TokenType::Identifier}, {}, uvVariableToken, outError))
        {
            return false;
        }
        ast::Type uvVarType;
        if (!m_Shader.FindVariable(uvVariableToken.value, uvVarType))
        {
            outError = {
                textureVariableToken.line, textureVariableToken.pos,
                std::format("Undefined variable: {}", textureVariableToken.value)
            };
            return false;
        }
        if (uvVarType != ast::Type::Vec2)
        {
            outError = {
                textureVariableToken.line, textureVariableToken.pos,
                std::format("Unexpected type: {} Expected: vec2", ast::TypeUtil::GetTypeGlsl(varType))
            };
            return false;
        }

        if (!ExpectAndConsume({TokenType::Syntax}, {")"}, outError))
        {
            return false;
        }

        m_Shader.AddNode(
            std::make_shared<ast::TextureSampleNode>(textureVariableToken.value, uvVariableToken.value));

        return true;
    }

    bool Parser::ProcessLengthFunc(const Token& token, ParseError& outError)
    {
        if (!ExpectAndConsume({TokenType::Syntax}, {"("}, outError))
        {
            return false;
        }

        auto length = std::make_shared<ast::LengthNode>();
        m_Shader.AddNode(length);
        m_Shader.PushScope(length);

        int componentsAccountedFor = 0;
        Token nextToken;
        while (true)
        {
            if (Peek({TokenType::Syntax}, {")"}))
            {
                break;
            }

            ast::Type argType;
            if (!ProcessExpression(argType, outError))
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
            case ast::Type::Sampler2D:
                outError = {
                    token.line, token.pos,
                    std::format("Unexpected type {}", ast::TypeUtil::GetTypeGlsl(argType))
                };
                return false;
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

        if (!ExpectedGetAndConsume({TokenType::Syntax}, {")"}, nextToken, outError))
        {
            return false;
        }

        if (componentsAccountedFor > 4)
        {
            outError = {
                nextToken.line, nextToken.pos, "Too many arguments for function call length"
            };
            return false;
        }

        m_Shader.PopScope();

        return true;
    }

    bool Parser::ProcessPowFunc(const Token& token, ParseError& outError)
    {
        if (!ExpectAndConsume({TokenType::Syntax}, {"("}, outError))
        {
            return false;
        }

        auto normalize = std::make_shared<ast::PowNode>();
        m_Shader.AddNode(normalize);
        m_Shader.PushScope(normalize);

        int argumentsAccountedFor = 0;
        Token nextToken;
        while (true)
        {
            if (Peek({TokenType::Syntax}, {")"}))
            {
                break;
            }

            ast::Type argType;
            if (!ProcessExpression(argType, outError))
            {
                return false;
            }

            switch (argType)
            {
            case ast::Type::Float:
                argumentsAccountedFor ++;
                break;
            default:
                outError = {
                    token.line, token.pos,
                    std::format("Unexpected type {}", ast::TypeUtil::GetTypeGlsl(argType))
                };
                return false;
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

        if (!ExpectedGetAndConsume({TokenType::Syntax}, {")"}, nextToken, outError))
        {
            return false;
        }

        if (argumentsAccountedFor != 2)
        {
            outError = {
                nextToken.line, nextToken.pos, "Too many arguments for function call length"
            };
            return false;
        }

        m_Shader.PopScope();

        return true;
    }

    bool Parser::ProcessNormalizeFunc(const Token& token, ast::Type& returnType, ParseError& outError)
    {
        if (!ExpectAndConsume({TokenType::Syntax}, {"("}, outError))
        {
            return false;
        }

        auto normalize = std::make_shared<ast::NormalizeNode>();
        m_Shader.AddNode(normalize);
        m_Shader.PushScope(normalize);

        int argumentsAccountedFor = 0;
        Token nextToken;
        while (true)
        {
            if (Peek({TokenType::Syntax}, {")"}))
            {
                break;
            }

            ast::Type argType;
            if (!ProcessExpression(argType, outError))
            {
                return false;
            }

            switch (argType)
            {
            case ast::Type::Vec2:
                returnType = ast::Type::Vec2;
                argumentsAccountedFor ++;
                break;
            case ast::Type::Vec3:
                returnType = ast::Type::Vec3;
                argumentsAccountedFor ++;
                break;
            case ast::Type::Vec4:
                returnType = ast::Type::Vec4;
                argumentsAccountedFor ++;
                break;
            case ast::Type::Float:
            case ast::Type::Mat3:
            case ast::Type::Mat4:
            case ast::Type::Void:
            case ast::Type::Invalid:
            case ast::Type::Sampler2D:
                outError = {
                    token.line, token.pos,
                    std::format("Unexpected type {}", ast::TypeUtil::GetTypeGlsl(argType))
                };
                return false;
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

        if (!ExpectedGetAndConsume({TokenType::Syntax}, {")"}, nextToken, outError))
        {
            return false;
        }

        if (argumentsAccountedFor != 1)
        {
            outError = {
                nextToken.line, nextToken.pos, "Too many arguments for function call length"
            };
            return false;
        }

        m_Shader.PopScope();

        return true;
    }

    bool Parser::ProcessReflectFunc(const Token& token, ast::Type& returnType, ParseError& outError)
    {
        if (!ExpectAndConsume({TokenType::Syntax}, {"("}, outError))
        {
            return false;
        }

        auto normalize = std::make_shared<ast::ReflectNode>();
        m_Shader.AddNode(normalize);
        m_Shader.PushScope(normalize);

        int argumentsAccountedFor = 0;
        Token nextToken;
        while (true)
        {
            if (Peek({TokenType::Syntax}, {")"}))
            {
                break;
            }

            ast::Type argType;
            if (!ProcessExpression(argType, outError))
            {
                return false;
            }

            switch (argType)
            {
            case ast::Type::Vec2:
                returnType = ast::Type::Vec2;
                argumentsAccountedFor ++;
                break;
            case ast::Type::Vec3:
                returnType = ast::Type::Vec3;
                argumentsAccountedFor ++;
                break;
            case ast::Type::Vec4:
                returnType = ast::Type::Vec4;
                argumentsAccountedFor ++;
                break;
            case ast::Type::Float:
            case ast::Type::Mat3:
            case ast::Type::Mat4:
            case ast::Type::Void:
            case ast::Type::Invalid:
            case ast::Type::Sampler2D:
                outError = {
                    token.line, token.pos,
                    std::format("Unexpected type {}", ast::TypeUtil::GetTypeGlsl(argType))
                };
                return false;
                break;
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

        if (!ExpectedGetAndConsume({TokenType::Syntax}, {")"}, nextToken, outError))
        {
            return false;
        }

        if (argumentsAccountedFor != 2)
        {
            outError = {
                nextToken.line, nextToken.pos, "Too many arguments for function call length"
            };
            return false;
        }

        m_Shader.PopScope();

        return true;
    }

    bool Parser::ProcessClampFunc(const Token& token, ParseError& outError)
    {
        if (!ExpectAndConsume({TokenType::Syntax}, {"("}, outError))
        {
            return false;
        }

        auto normalize = std::make_shared<ast::ClampNode>();
        m_Shader.AddNode(normalize);
        m_Shader.PushScope(normalize);

        Token nextToken;
        int componentsAccountedFor = 0;
        while (true)
        {
            if (Peek({TokenType::Syntax}, {")"}))
            {
                break;
            }

            ast::Type argType;
            if (!ProcessExpression(argType, outError))
            {
                return false;
            }

            switch (argType)
            {
            case ast::Type::Float:
                componentsAccountedFor++;
                break;
            default:
                outError = {
                    token.line, token.pos,
                    std::format("Unexpected type {}", ast::TypeUtil::GetTypeGlsl(argType))
                };
                return false;
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

        if (componentsAccountedFor != 3)
        {
            outError = {
                nextToken.line, nextToken.pos, "wrong number of arguments for function call clamp"
            };
            return false;
        }

        if (!ExpectedGetAndConsume({TokenType::Syntax}, {")"}, nextToken, outError))
        {
            return false;
        }

        m_Shader.PopScope();

        return true;
    }

    bool Parser::ProcessDotFunc(const Token& token, ParseError& outError)
    {
        if (!ExpectAndConsume({TokenType::Syntax}, {"("}, outError))
        {
            return false;
        }

        auto dot = std::make_shared<ast::DotNode>();
        m_Shader.AddNode(dot);
        m_Shader.PushScope(dot);

        Token nextToken;
        int argumentsAccountedFor = 0;
        while (true)
        {
            if (Peek({TokenType::Syntax}, {")"}))
            {
                break;
            }

            ast::Type argType;
            if (!ProcessExpression(argType, outError))
            {
                return false;
            }

            switch (argType)
            {
            case ast::Type::Vec2:
            case ast::Type::Vec3:
            case ast::Type::Vec4:
                argumentsAccountedFor++;
                break;
            default:
                outError = {
                token.line, token.pos,
                std::format("Unexpected type {}", ast::TypeUtil::GetTypeGlsl(argType))
            };
                return false;
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

        if (argumentsAccountedFor != 2)
        {
            outError = {
                nextToken.line, nextToken.pos, "wrong number of arguments for function call clamp"
            };
            return false;
        }

        if (!ExpectedGetAndConsume({TokenType::Syntax}, {")"}, nextToken, outError))
        {
            return false;
        }

        m_Shader.PopScope();

        return true;
    }

    bool Parser::ProcessPropertyAccess(const Token&, ast::Type& returnType, ParseError& outError)
    {
        if (!ExpectAndConsume({TokenType::Syntax}, {"."}, outError))
        {
            return false;
        }

        Token propertyNameToken;
        if (!ExpectedGetAndConsume({TokenType::Identifier}, {}, propertyNameToken, outError))
        {
            return false;
        }

        //TODO make this a function
        if (propertyNameToken.value == "x" ||
            propertyNameToken.value == "y" ||
            propertyNameToken.value == "z" ||
            propertyNameToken.value == "r" ||
            propertyNameToken.value == "g" ||
            propertyNameToken.value == "b")
        {
            returnType = ast::Type::Float;
        }
        else if (propertyNameToken.value == "xyz" ||
                 propertyNameToken.value == "rgb")
        {
            returnType = ast::Type::Vec3;
        }
        else
        {
            outError = {
                propertyNameToken.line, propertyNameToken.pos,
                std::format("Unhandled property access {}", propertyNameToken.value)
            };
            return false;
        }

        auto propertyAccessNode = std::make_shared<ast::PropertyAccessNode>(propertyNameToken.value);
        m_Shader.AddNode(propertyAccessNode);
        m_Shader.PushScope(propertyAccessNode);
        return true;
    }
}
