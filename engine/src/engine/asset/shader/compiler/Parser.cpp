#include <engine/asset/shader/ast/IfNode.h>
#include <engine/asset/shader/ast/ElseNode.h>
#include "Parser.h"

#include "engine/asset/shader/ast/AbsNode.h"
#include "engine/asset/shader/ast/TextureSampleNode.h"
#include "engine/asset/shader/ast/PropertyAccessNode.h"
#include "engine/asset/shader/ast/SmoothstepNode.h"
#include "engine/asset/shader/ast/AnonymousScopeNode.h"
#include "engine/asset/shader/ast/BinaryExpressionNode.h"
#include "engine/asset/shader/ast/ClampNode.h"
#include "engine/asset/shader/ast/ConstantNode.h"
#include "engine/asset/shader/ast/DFDXNode.h"
#include "engine/asset/shader/ast/DFDYNode.h"
#include "engine/asset/shader/ast/DotNode.h"
#include "engine/asset/shader/ast/EndOfExpressionNode.h"
#include "engine/asset/shader/ast/ForLoopNode.h"
#include "engine/asset/shader/ast/FWidthNode.h"
#include "engine/asset/shader/ast/InputPortNode.h"
#include "engine/asset/shader/ast/InverseNode.h"
#include "engine/asset/shader/ast/LengthNode.h"
#include "engine/asset/shader/ast/MainNode.h"
#include "engine/asset/shader/ast/MaxNode.h"
#include "engine/asset/shader/ast/MinNode.h"
#include "engine/asset/shader/ast/ModNode.h"
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

    std::variant<Shader, ParseError> Parser::Parse()
    {
        ParseError error;
        Token shaderTypeToken;
        if (!ExpectedGetAndConsume({TokenType::Builtin}, {"SSL_VERTEX", "SSL_FRAGMENT"}, shaderTypeToken, error))
        {
            return error;
        }

        if (shaderTypeToken.value == "SSL_VERTEX")
        {
            m_Shader.SetType(ShaderType::Vertex);
        }
        else
        {
            m_Shader.SetType(ShaderType::Fragment);
        }

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
                    ast::AstType builtinType;
                    if (!ProcessBuiltin(token, builtinType, error))
                    {
                        return error;
                    }
                    break;
                case TokenType::Identifier:
                    ast::AstType type;
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

        auto vec2 = m_Shader.AddNode<ast::Vec2Node>();
        m_Shader.PushScope(vec2);
        int componentsAccountedFor = 0;
        while (componentsAccountedFor < 2)
        {
            ast::AstType argType;
            if (!ProcessExpression(argType, outError))
            {
                return false;
            }

            switch (argType)
            {
            case ast::AstType::Int:
            case ast::AstType::Float:
                componentsAccountedFor++;
                break;
            case ast::AstType::Vec2:
                componentsAccountedFor += 2;
                break;
            default:
                outError = {
                    nextToken.line, nextToken.pos,
                    std::format("Unexpected type {}", ast::TypeUtil::TypeToGlsl(argType))
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

        auto vec3 = m_Shader.AddNode<ast::Vec3Node>();
        m_Shader.PushScope(vec3);
        int componentsAccountedFor = 0;
        while (componentsAccountedFor < 3)
        {
            ast::AstType argType;
            if (!ProcessExpression(argType, outError))
            {
                return false;
            }

            switch (argType)
            {
            case ast::AstType::Int:
            case ast::AstType::Float:
                componentsAccountedFor++;
                break;
            case ast::AstType::Vec2:
                componentsAccountedFor += 2;
                break;
            case ast::AstType::Vec3:
            case ast::AstType::Vec4:
                componentsAccountedFor += 3;
                break;
            case ast::AstType::Mat4:
            {
                Token token;
                if (!ExpectedGet({TokenType::Syntax}, {"*"}, token, outError))
                {
                    return false;
                }
                break;
            }
            default:
                outError = {
                    nextToken.line, nextToken.pos,
                    std::format("Unexpected type {}", ast::TypeUtil::TypeToGlsl(argType))
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

        auto vec4 = m_Shader.AddNode<ast::Vec4Node>();
        m_Shader.PushScope(vec4);
        int componentsAccountedFor = 0;
        while (componentsAccountedFor < 4)
        {
            ast::AstType argType;
            if (!ProcessExpression(argType, outError))
            {
                return false;
            }

            switch (argType)
            {
            case ast::AstType::Int:
            case ast::AstType::Float:
                componentsAccountedFor++;
                break;
            case ast::AstType::Vec2:
                componentsAccountedFor += 2;
                break;
            case ast::AstType::Vec3:
                componentsAccountedFor += 3;
                break;
            case ast::AstType::Vec4:
                componentsAccountedFor += 4;
                break;
            default:
                outError = {
                    nextToken.line, nextToken.pos,
                    std::format("Unexpected type {}", ast::TypeUtil::TypeToGlsl(argType))
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

    bool Parser::ProcessBuiltin(const Token& token, ast::AstType& returnType, ParseError& outError)
    {
        returnType = ast::AstType::Invalid;

        bool isPossibleVariableDec = ast::TypeUtil::GlslToType(token.value) != ast::AstType::Invalid;
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
            returnType = ast::AstType::Void;
            return ProcessFunctionDeclaration(token, outError);
        }
        else if (token.value == "vec2")
        {
            returnType = ast::AstType::Vec2;
            return ProcessVec2(token, outError);
        }
        else if (token.value == "vec3")
        {
            returnType = ast::AstType::Vec3;
            return ProcessVec3(token, outError);
        }
        else if (token.value == "vec4")
        {
            returnType = ast::AstType::Vec4;
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
        else if (token.value == "setting")
        {
            return ProcessSettingDeclaration(token, returnType, outError);
        }
        else if (token.value == "texture")
        {
            returnType = ast::AstType::Vec4;
            return ProcessTextureRead(token, outError);
        }
        else if (token.value == "length")
        {
            returnType = ast::AstType::Float;
            return ProcessLengthFunc(token, outError);
        }
        else if (token.value == "normalize")
        {
            return ProcessNormalizeFunc(token, returnType, outError);
        }
        else if (token.value == "smoothstep")
        {
            return ProcessSmoothstepFunc(token, returnType, outError);
        }
        else if (token.value == "clamp")
        {
            returnType = ast::AstType::Float;
            return ProcessClampFunc(token, outError);
        }
        else if (token.value == "dot")
        {
            returnType = ast::AstType::Float;
            return ProcessDotFunc(token, outError);
        }
        else if (token.value == "reflect")
        {
            return ProcessReflectFunc(token, returnType, outError);
        }
        else if (token.value == "pow")
        {
            returnType = ast::AstType::Float;
            return ProcessPowFunc(token, outError);
        }
        else if (token.value == "for")
        {
            returnType = ast::AstType::Void;
            return ProcessForLoop(outError);
        }
        else if (token.value == "if")
        {
            returnType = ast::AstType::Void;
            return ProcessIfStatement(outError);
        }
        else if (token.value == "else")
        {
            returnType = ast::AstType::Void;
            return ProcessElseStatement(outError);
        }
        else if (token.value == "dfdx")
        {
            returnType = ast::AstType::Float;
            return ProcessDFDXStatement(token, outError);
        }
        else if (token.value == "dfdy")
        {
            returnType = ast::AstType::Float;
            return ProcessDFDYStatement(token, outError);
        }
        else if (token.value == "mod")
        {
            return ProcessModFunc(token, returnType, outError);
        }
        else if (token.value == "abs")
        {
            return ProcessAbsFunc(token, returnType, outError);
        }
        else if (token.value == "fwidth")
        {
            return ProcessFWidthFunc(token, returnType, outError);
        }
        else if (token.value == "inverse")
        {
            return ProcessInverseFunc(token, returnType, outError);
        }
        else if (token.value == "min")
        {
            return ProcessMinFunc(token, returnType, outError);
        }
        else if (token.value == "max")
        {
            return ProcessMaxFunc(token, returnType, outError);
        }

        outError = {token.line, token.pos, std::format("Unexpected token {}", token.value)};
        SPARK_ASSERT(false);
        return false;
    }

    bool Parser::ProcessNumericLiteral(const Token& token, ParseError&)
    {
        if (IsInteger(token.value))
        {
            m_Shader.AddNode<ast::ConstantNode<int>>(std::stoi(token.value));
        }
        else
        {
            m_Shader.AddNode<ast::ConstantNode<float>>(std::stof(token.value));

            auto peek = m_Lexer.PeekToken();
            if (std::holds_alternative<Token>(peek))
            {
                auto nextToken = std::get<Token>(peek);
                if (nextToken.value == "f")
                {
                    m_Lexer.ConsumeToken();
                }
            }
        }
        return true;
    }

    bool Parser::ProcessStringLiteral(const Token& token, ParseError& outError)
    {
        outError.error = "String literals not supported.";
        outError.line = token.line;
        outError.pos = token.pos;

        return false;
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
            auto scope = m_Shader.AddNode<ast::AnonymousScopeNode>();
            m_Shader.PushScope(scope);
            return true;
        }
        else if (token.value == ";")
        {
            m_Shader.AddNode<ast::EndOfExpressionNode>();
            return true;
        }
        else
        {
            outError = {token.line, token.pos, std::format("Parser::ProcessSyntax - unhandled token {}", token.value)};
            return false;
        }
    }

    bool Parser::ProcessPortDeclaration(const Token&, ast::AstType& returnType, ParseError& outError)
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
        ast::AstType type = ast::TypeUtil::GlslToType(typeToken.value);
        returnType = type;

        auto var = ast::Variable(type, 0);
        if (Peek({TokenType::Syntax}, {"["}))
        {
            //array
            if (!ExpectAndConsume({TokenType::Syntax}, {"["}, outError))
            {
                return false;
            }

            Token sizeArg;
            if (!ExpectedGetAndConsume({TokenType::Identifier, TokenType::NumericLiteral}, {}, sizeArg, outError))
            {
                return false;
            }

            if (sizeArg.type == TokenType::NumericLiteral)
            {
                var.arraySizeConstant = std::stoi(sizeArg.value);
            }
            else
            {
                var.arraySizeVariable = sizeArg.value;
            }

            if (!ExpectAndConsume({TokenType::Syntax}, {"]"}, outError))
            {
                return false;
            }
        }

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
            m_Shader.AddInputPort(std::make_shared<ast::InputPortNode>(portName.value, var, name));
        }
        else
        {
            m_Shader.AddOutputPort(std::make_shared<ast::OutputPortNode>(portName.value, var, name));
        }

        return true;
    }

    bool Parser::ProcessUniformDeclaration(const Token&, ast::AstType& returnType, ParseError& outError)
    {
        Token typeToken;
        if (!ExpectedGetAndConsume({TokenType::Builtin}, ast::TypeUtil::GetTypeStrings(), typeToken,
                                   outError))
        {
            return false;
        }
        ast::AstType type = ast::TypeUtil::GlslToType(typeToken.value);
        returnType = type;

        Token nameToken;
        if (!ExpectedGetAndConsume({TokenType::Identifier}, {}, nameToken, outError))
        {
            return false;
        }
        std::string name = nameToken.value;

        auto var = ast::Variable(type, 0);
        if (Peek({TokenType::Syntax}, {"["}))
        {
            //array
            if (!ExpectAndConsume({TokenType::Syntax}, {"["}, outError))
            {
                return false;
            }

            Token sizeArg;
            if (!ExpectedGetAndConsume({TokenType::Identifier, TokenType::NumericLiteral}, {}, sizeArg, outError))
            {
                return false;
            }

            if (sizeArg.type == TokenType::NumericLiteral)
            {
                var.arraySizeConstant = std::stoi(sizeArg.value);
            }
            else
            {
                var.arraySizeVariable = sizeArg.value;
            }

            if (!ExpectAndConsume({TokenType::Syntax}, {"]"}, outError))
            {
                return false;
            }
        }

        if (!ExpectAndConsume({TokenType::Syntax}, {";"}, outError))
        {
            return false;
        }

        std::string temp;
        if (!m_Shader.AddUniform(name, var, temp))
        {
            outError = {nameToken.line, nameToken.pos, temp.data()};
            return false;
        }

        return true;
    }

    bool Parser::ProcessSettingDeclaration(const Token&, ast::AstType& returnType, ParseError& outError)
    {
        Token typeToken;
        if (!ExpectedGetAndConsume({TokenType::Builtin}, ast::TypeUtil::GetTypeStrings(), typeToken,
                                   outError))
        {
            return false;
        }
        ast::AstType type = ast::TypeUtil::GlslToType(typeToken.value);
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
        if (!m_Shader.AddSetting(name, ast::Variable(type, 0), temp))
        {
            outError = {nameToken.line, nameToken.pos, temp.data()};
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
                    auto main = m_Shader.AddNode<ast::MainNode>();
                    m_Shader.PushScope(main);
                }
                else
                {
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

        ast::AstType declarationType = ast::TypeUtil::GlslToType(token.value);
        m_Shader.AddNode<ast::VariableDeclarationNode>(nameToken.value, ast::Variable(declarationType, 0));
        m_Shader.AddNode<ast::EndOfExpressionNode>();

        std::string error;
        if (!m_Shader.RecordVariableForScope(nameToken.value, ast::Variable(declarationType, 0), error))
        {
            outError = {nameToken.line, nameToken.pos, error.data()};
            return false;
        }

        ast::AstType expressionType;
        if (!ProcessExpression(expressionType, outError))
        {
            return false;
        }

        if (declarationType != expressionType)
        {
            outError = {token.line, token.pos, std::format("Cannot assign value of type {} to {}", token.value, ast::TypeUtil::TypeToGlsl(expressionType))};
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

    bool Parser::ProcessExpression(ast::AstType& outType, ParseError& outError)
    {
        ast::AstType expressionType = ast::AstType::Invalid;
        int numBinaryExpressions = 0;
        while (true)
        {
            Token binaryOpToken;
            if (Peek({ TokenType::Syntax }, ast::OperatorUtil::GetOperatorStrings(), binaryOpToken))
            {
                auto binaryOp = m_Shader.AddNode<ast::BinaryExpressionNode>(ast::OperatorUtil::StringToOperatorType(binaryOpToken.value));
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

            // handle array access in binary op
            if (Peek(binaryOpPeekOffset, {TokenType::Syntax}, {"["}))
            {
                binaryOpPeekOffset += 3; // skip array access
            }

            if (Peek(binaryOpPeekOffset, { TokenType::Syntax }, ast::OperatorUtil::GetOperatorStrings(), binaryOpToken))
            {
                auto opType = ast::OperatorUtil::StringToOperatorType(binaryOpToken.value);
                if (opType == ast::OperatorType::Less ||
                    opType == ast::OperatorType::LessEquals ||
                    opType == ast::OperatorType::Greater ||
                    opType == ast::OperatorType::GreaterEquals ||
                    opType == ast::OperatorType::Compare)
                {
                    expressionType = ast::AstType::Bool;
                }
                auto binaryOp = m_Shader.AddNode<ast::BinaryExpressionNode>(opType);
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
                auto varInfo = m_Shader.FindVariable(nextToken.value);
                if (!varInfo.has_value())
                {
                    outError = {nextToken.line, nextToken.pos, std::format("Undefined variable {}", nextToken.value)};
                    return false;
                }

                // check for property access
                bool isPropertyAccess = false;
                auto peek = m_Lexer.PeekToken();
                if (std::holds_alternative<Token>(peek) && std::get<Token>(peek).value == ".")
                {
                    ast::AstType propertyType;
                    if (!ProcessPropertyAccess(nextToken, propertyType, outError))
                    {
                        return false;
                    }

                    isPropertyAccess = true;

                    if (expressionType == ast::AstType::Invalid)
                    {
                        expressionType = propertyType;
                    }
                }
                else if (expressionType == ast::AstType::Invalid)
                {
                    expressionType = varInfo.value().type;
                }

                int arrayIndex = -1;
                std::string arrayIndexVar = "";
                if (Peek({TokenType::Syntax}, {"["}))
                {
                    //array
                    if (!ExpectAndConsume({TokenType::Syntax}, {"["}, outError))
                    {
                        return false;
                    }

                    Token sizeArg;
                    if (!ExpectedGetAndConsume({TokenType::Identifier, TokenType::NumericLiteral}, {}, sizeArg, outError))
                    {
                        return false;
                    }

                    if (sizeArg.type == TokenType::NumericLiteral)
                    {
                        arrayIndex = std::stoi(sizeArg.value);
                    }
                    else
                    {
                        arrayIndexVar = sizeArg.value;
                    }

                    if (!ExpectAndConsume({TokenType::Syntax}, {"]"}, outError))
                    {
                        return false;
                    }


                    if (varInfo.value().arraySizeConstant == 0 && varInfo->arraySizeVariable.empty())
                    {
                        if (expressionType == ast::AstType::Vec2)
                        {
                            expressionType =  ast::AstType::Float;
                        }
                        else if (expressionType == ast::AstType::Vec3)
                        {
                            expressionType = ast::AstType::Float;
                        }
                        else if (expressionType == ast::AstType::Vec4)
                        {
                            expressionType = ast::AstType::Float;
                        }
                        else if (expressionType == ast::AstType::Mat3)
                        {
                            expressionType = ast::AstType::Vec3;
                        }
                        else if (expressionType == ast::AstType::Mat4)
                        {
                            expressionType = ast::AstType::Vec4;
                        }
                    }
                }

                m_Shader.AddNode<ast::VariableReferenceNode>(nextToken.value, m_Shader, arrayIndex, arrayIndexVar);
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
                if (expressionType == ast::AstType::Invalid)
                {
                    expressionType = IsInteger(nextToken.value) ? ast::AstType::Int : ast::AstType::Float;
                }
            }
            else if (nextToken.type == TokenType::StringLiteral)
            {
                if (!ProcessStringLiteral(nextToken, outError))
                {
                    return false;
                }
            }
            else if (nextToken.type == TokenType::Builtin)
            {
                ast::AstType builtinType;
                if (!ProcessBuiltin(nextToken, builtinType, outError))
                {
                    return false;
                }

                // check for property access
                bool isPropertyAccess = false;
                auto peek = m_Lexer.PeekToken();
                if (std::holds_alternative<Token>(peek) && std::get<Token>(peek).value == ".")
                {
                    if (!ProcessPropertyAccess(nextToken, expressionType, outError))
                    {
                        return false;
                    }

                    isPropertyAccess = true;
                }
                else if (expressionType == ast::AstType::Invalid)
                {
                    expressionType = builtinType;
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
                if (SPARK_VERIFY(Peek({ TokenType::Syntax }, ast::OperatorUtil::GetOperatorStrings())))
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

        outType = expressionType;

        return true;
    }

    bool Parser::ProcessForLoop(ParseError& outError)
    {
        auto forLoop = m_Shader.AddNode<ast::ForLoopNode>();
        m_Shader.PushScope(forLoop);

        if (!ExpectAndConsume({TokenType::Syntax}, {"("}, outError))
        {
            return false;
        }

        Token itToken;
        if (!ExpectedGetAndConsume({TokenType::Builtin}, {}, itToken, outError))
        {
            return false;
        }

        auto itType = ast::TypeUtil::GlslToType(itToken.value);
        if (itType == ast::AstType::Invalid)
        {
            outError = { itToken.line, itToken.pos, std::format("Unexpected token {}", itToken.value) };
            return false;
        }

        if (!ProcessVariableDeclaration(itToken, outError))
        {
            return false;
        }

        if (!ExpectAndConsume({TokenType::Syntax}, {";"}, outError))
        {
            return false;
        }

        forLoop->m_DeclarationEnded = true;

        ast::AstType expression1Type;
        if (!ProcessExpression(expression1Type, outError))
        {
            return false;
        }

        if (!ExpectAndConsume({TokenType::Syntax}, {";"}, outError))
        {
            return false;
        }

        ast::AstType expression2Type;
        if (!ProcessExpression(expression2Type, outError))
        {
            return false;
        }

        if (!ExpectAndConsume({TokenType::Syntax}, {")"}, outError))
        {
            return false;
        }

        forLoop->m_ExpressionEnded = true;

        if (!ExpectAndConsume({TokenType::Syntax}, {"{"}, outError))
        {
            return false;
        }

        return true;
    }

    bool Parser::ProcessIfStatement(ParseError& outError)
    {
        auto ifNode = m_Shader.AddNode<ast::IfNode>();
        m_Shader.PushScope(ifNode);

        if (!ExpectAndConsume({TokenType::Syntax}, {"("}, outError))
        {
            return false;
        }

        ast::AstType expressionType;
        if (!ProcessExpression(expressionType, outError))
        {
            return false;
        }

        if (expressionType != ast::AstType::Bool)
        {
            outError = { .line = 0, .pos = 0, .error = "If statement expression returning non bool type not supported."};
            return false;
        }

        if (!ExpectAndConsume({TokenType::Syntax}, {")"}, outError))
        {
            return false;
        }

        if (!ExpectAndConsume({TokenType::Syntax}, {"{"}, outError))
        {
            return false;
        }

        return true;
    }

    bool Parser::ProcessElseStatement(ParseError& outError)
    {
        auto ifNode = m_Shader.AddNode<ast::ElseNode>();
        m_Shader.PushScope(ifNode);

        if (!ExpectAndConsume({TokenType::Syntax}, {"{"}, outError))
        {
            return false;
        }

        return true;
    }

    bool Parser::ProcessDFDXStatement(const Token& token, ParseError& outError)
    {
        if (!ExpectAndConsume({TokenType::Syntax}, {"("}, outError))
        {
            return false;
        }

        auto dfdx = m_Shader.AddNode<ast::DFDXNode>();
        m_Shader.PushScope(dfdx);

        int argumentsAccountedFor = 0;
        Token nextToken;
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

            switch (argType)
            {
                case ast::AstType::Float:
                    argumentsAccountedFor++;
                    break;
                default:
                    outError = {
                    token.line, token.pos,
                    std::format("Unexpected type {}", ast::TypeUtil::TypeToGlsl(argType))
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
                nextToken.line, nextToken.pos, "Incorrect number of arguments for function call dfdx"
            };
            return false;
        }

        m_Shader.PopScope();

        return true;
    }

    bool Parser::ProcessDFDYStatement(const Token& token, ParseError& outError)
    {
        if (!ExpectAndConsume({TokenType::Syntax}, {"("}, outError))
        {
            return false;
        }

        auto dfdy = m_Shader.AddNode<ast::DFDYNode>();
        m_Shader.PushScope(dfdy);

        int argumentsAccountedFor = 0;
        Token nextToken;
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

            switch (argType)
            {
                case ast::AstType::Float:
                    argumentsAccountedFor++;
                    break;
                default:
                    outError = {
                    token.line, token.pos,
                    std::format("Unexpected type {}", ast::TypeUtil::TypeToGlsl(argType))
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
                nextToken.line, nextToken.pos, "Incorrect number of arguments for function call dfdx"
            };
            return false;
        }

        m_Shader.PopScope();

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
        auto texVarInfo = m_Shader.FindVariable(textureVariableToken.value);
        if (!texVarInfo.has_value())
        {
            outError = {
                textureVariableToken.line, textureVariableToken.pos,
                std::format("Undefined variable: {}", textureVariableToken.value)
            };
            return false;
        }
        if (texVarInfo.value().type != ast::AstType::Sampler2D)
        {
            outError = {
                textureVariableToken.line, textureVariableToken.pos,
                std::format("Unexpected type: {} Expected: sampler2D", ast::TypeUtil::TypeToGlsl(texVarInfo.value().type))
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

        auto uvVarInfo = m_Shader.FindVariable(uvVariableToken.value);
        if (!uvVarInfo.has_value())
        {
            outError = {
                textureVariableToken.line, textureVariableToken.pos,
                std::format("Undefined variable: {}", textureVariableToken.value)
            };
            return false;
        }
        if (uvVarInfo.value().type != ast::AstType::Vec2)
        {
            outError = {
                textureVariableToken.line, textureVariableToken.pos,
                std::format("Unexpected type: {} Expected: vec2", ast::TypeUtil::TypeToGlsl(uvVarInfo.value().type))
            };
            return false;
        }

        if (!ExpectAndConsume({TokenType::Syntax}, {")"}, outError))
        {
            return false;
        }

        m_Shader.AddNode<ast::TextureSampleNode>(textureVariableToken.value, uvVariableToken.value);

        return true;
    }

    bool Parser::ProcessLengthFunc(const Token& token, ParseError& outError)
    {
        if (!ExpectAndConsume({TokenType::Syntax}, {"("}, outError))
        {
            return false;
        }

        auto length = m_Shader.AddNode<ast::LengthNode>();
        m_Shader.PushScope(length);

        int componentsAccountedFor = 0;
        Token nextToken;
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

            switch (argType)
            {
            case ast::AstType::Int:
            case ast::AstType::Float:
                componentsAccountedFor++;
                break;
            case ast::AstType::Vec2:
                componentsAccountedFor += 2;
                break;
            case ast::AstType::Vec3:
                componentsAccountedFor += 3;
                break;
            case ast::AstType::Vec4:
                componentsAccountedFor += 4;
                break;
            default:
                outError = {
                    token.line, token.pos,
                    std::format("Unexpected type {}", ast::TypeUtil::TypeToGlsl(argType))
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

        auto pow = m_Shader.AddNode<ast::PowNode>();
        m_Shader.PushScope(pow);

        int argumentsAccountedFor = 0;
        Token nextToken;
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

            switch (argType)
            {
            case ast::AstType::Int:
            case ast::AstType::Float:
                argumentsAccountedFor ++;
                break;
            default:
                outError = {
                    token.line, token.pos,
                    std::format("Unexpected type {}", ast::TypeUtil::TypeToGlsl(argType))
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

    bool Parser::ProcessNormalizeFunc(const Token& token, ast::AstType& returnType, ParseError& outError)
    {
        if (!ExpectAndConsume({TokenType::Syntax}, {"("}, outError))
        {
            return false;
        }

        auto normalize = m_Shader.AddNode<ast::NormalizeNode>();
        m_Shader.PushScope(normalize);

        int argumentsAccountedFor = 0;
        Token nextToken;
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

            switch (argType)
            {
            case ast::AstType::Vec2:
                returnType = ast::AstType::Vec2;
                argumentsAccountedFor ++;
                break;
            case ast::AstType::Vec3:
                returnType = ast::AstType::Vec3;
                argumentsAccountedFor ++;
                break;
            case ast::AstType::Vec4:
                returnType = ast::AstType::Vec4;
                argumentsAccountedFor ++;
                break;
            default:
                outError = {
                    token.line, token.pos,
                    std::format("Unexpected type {}", ast::TypeUtil::TypeToGlsl(argType))
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

    bool Parser::ProcessSmoothstepFunc(const Token& token,
                                       ast::AstType& returnType,
                                       ParseError& outError)
    {
        if (!ExpectAndConsume({TokenType::Syntax}, {"("}, outError))
        {
            return false;
        }

        auto smoothstep = m_Shader.AddNode<ast::SmoothstepNode>();
        m_Shader.PushScope(smoothstep);

        int argumentsAccountedFor = 0;
        Token nextToken;
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

            switch (argType)
            {
                case ast::AstType::Float:
                    if (argumentsAccountedFor == 2)
                    {
                        returnType = ast::AstType::Float;
                    }
                    argumentsAccountedFor++;
                    break;
                case ast::AstType::Vec2:
                    if (argumentsAccountedFor == 2)
                    {
                        returnType = ast::AstType::Vec2;
                    }
                    argumentsAccountedFor++;
                    break;
                default:
                    outError = {
                            token.line, token.pos,
                            std::format("Unexpected type {}", ast::TypeUtil::TypeToGlsl(argType))
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

        if (argumentsAccountedFor != 3)
        {
            outError = {
                    nextToken.line, nextToken.pos, "Incorrect number of arguments for function call smoothstep"
            };
            return false;
        }

        m_Shader.PopScope();

        return true;
    }

    bool Parser::ProcessReflectFunc(const Token& token, ast::AstType& returnType, ParseError& outError)
    {
        if (!ExpectAndConsume({TokenType::Syntax}, {"("}, outError))
        {
            return false;
        }

        auto reflect = m_Shader.AddNode<ast::ReflectNode>();
        m_Shader.PushScope(reflect);

        int argumentsAccountedFor = 0;
        Token nextToken;
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

            switch (argType)
            {
            case ast::AstType::Vec2:
                returnType = ast::AstType::Vec2;
                argumentsAccountedFor ++;
                break;
            case ast::AstType::Vec3:
                returnType = ast::AstType::Vec3;
                argumentsAccountedFor ++;
                break;
            case ast::AstType::Vec4:
                returnType = ast::AstType::Vec4;
                argumentsAccountedFor ++;
                break;
            default:
                outError = {
                    token.line, token.pos,
                    std::format("Unexpected type {}", ast::TypeUtil::TypeToGlsl(argType))
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

        auto clamp = m_Shader.AddNode<ast::ClampNode>();
        m_Shader.PushScope(clamp);

        Token nextToken;
        int componentsAccountedFor = 0;
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

            switch (argType)
            {
            case ast::AstType::Int:
            case ast::AstType::Float:
                componentsAccountedFor++;
                break;
            default:
                outError = {
                    token.line, token.pos,
                    std::format("Unexpected type {}", ast::TypeUtil::TypeToGlsl(argType))
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

        auto dot = m_Shader.AddNode<ast::DotNode>();
        m_Shader.PushScope(dot);

        Token nextToken;
        int argumentsAccountedFor = 0;
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

            switch (argType)
            {
            case ast::AstType::Vec2:
            case ast::AstType::Vec3:
            case ast::AstType::Vec4:
                argumentsAccountedFor++;
                break;
            default:
                outError = {
                token.line, token.pos,
                std::format("Unexpected type {}", ast::TypeUtil::TypeToGlsl(argType))
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

    bool Parser::ProcessPropertyAccess(const Token&, ast::AstType& returnType, ParseError& outError)
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

        if (propertyNameToken.value == "x" ||
            propertyNameToken.value == "y" ||
            propertyNameToken.value == "z" ||
            propertyNameToken.value == "w" ||
            propertyNameToken.value == "r" ||
            propertyNameToken.value == "g" ||
            propertyNameToken.value == "b" ||
            propertyNameToken.value == "a")
        {
            returnType = ast::AstType::Float;
        }
        else if (propertyNameToken.value == "xy" ||
                 propertyNameToken.value == "xz")
        {
            returnType = ast::AstType::Vec2;
        }
        else if (propertyNameToken.value == "xyz" ||
                 propertyNameToken.value == "rgb")
        {
            returnType = ast::AstType::Vec3;
        }
        else
        {
            outError = {
                propertyNameToken.line, propertyNameToken.pos,
                std::format("Unhandled property access {}", propertyNameToken.value)
            };
            return false;
        }

        auto propertyAccessNode = m_Shader.AddNode<ast::PropertyAccessNode>(propertyNameToken.value);
        m_Shader.PushScope(propertyAccessNode);
        return true;
    }

    bool Parser::ProcessModFunc(const Token& token,
        ast::AstType& returnType,
        ParseError& outError)
    {
        if (!ExpectAndConsume({TokenType::Syntax}, {"("}, outError))
        {
            return false;
        }

        auto mod = m_Shader.AddNode<ast::ModNode>();
        m_Shader.PushScope(mod);

        int argumentsAccountedFor = 0;
        Token nextToken;
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

            switch (argType)
            {
                case ast::AstType::Float:
                    returnType = ast::AstType::Float;
                    argumentsAccountedFor ++;
                    break;
                case ast::AstType::Vec2:
                    returnType = ast::AstType::Vec2;
                    argumentsAccountedFor ++;
                    break;
                case ast::AstType::Vec3:
                    returnType = ast::AstType::Vec3;
                    argumentsAccountedFor ++;
                    break;
                case ast::AstType::Vec4:
                    returnType = ast::AstType::Vec4;
                    argumentsAccountedFor ++;
                    break;
                default:
                    outError = {
                    token.line, token.pos,
                    std::format("Unexpected type {}", ast::TypeUtil::TypeToGlsl(argType))
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
                nextToken.line, nextToken.pos, "Too many arguments for function call mod"
            };
            return false;
        }

        m_Shader.PopScope();

        return true;
    }

    bool Parser::ProcessAbsFunc(const Token& token,
        ast::AstType& returnType,
        ParseError& outError)
    {
        if (!ExpectAndConsume({TokenType::Syntax}, {"("}, outError))
        {
            return false;
        }

        auto abs = m_Shader.AddNode<ast::AbsNode>();
        m_Shader.PushScope(abs);

        int argumentsAccountedFor = 0;
        Token nextToken;
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

            switch (argType)
            {
                case ast::AstType::Float:
                    returnType = ast::AstType::Float;
                    argumentsAccountedFor ++;
                    break;
                case ast::AstType::Vec2:
                    returnType = ast::AstType::Vec2;
                    argumentsAccountedFor ++;
                    break;
                case ast::AstType::Vec3:
                    returnType = ast::AstType::Vec3;
                    argumentsAccountedFor ++;
                    break;
                case ast::AstType::Vec4:
                    returnType = ast::AstType::Vec4;
                    argumentsAccountedFor ++;
                    break;
                default:
                    outError = {
                    token.line, token.pos,
                    std::format("Unexpected type {}", ast::TypeUtil::TypeToGlsl(argType))
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

        if (argumentsAccountedFor != 1)
        {
            outError = {
                nextToken.line, nextToken.pos, "Too many arguments for function call abs"
            };
            return false;
        }

        m_Shader.PopScope();

        return true;
    }

    bool Parser::ProcessFWidthFunc(const Token& token,
        ast::AstType& returnType,
        ParseError& outError)
    {
        returnType = ast::AstType::Vec2;

        if (!ExpectAndConsume({TokenType::Syntax}, {"("}, outError))
        {
            return false;
        }

        auto fwidth = m_Shader.AddNode<ast::FWidthNode>();
        m_Shader.PushScope(fwidth);

        int argumentsAccountedFor = 0;
        Token nextToken;
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

            switch (argType)
            {
                case ast::AstType::Vec2:
                    argumentsAccountedFor ++;
                    break;
                default:
                    outError = {
                    token.line, token.pos,
                    std::format("Unexpected type {}", ast::TypeUtil::TypeToGlsl(argType))
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

        if (argumentsAccountedFor != 1)
        {
            outError = {
                nextToken.line, nextToken.pos, "Too many arguments for function call abs"
            };
            return false;
        }

        m_Shader.PopScope();

        return true;
    }

    bool Parser::ProcessInverseFunc(const Token& token,
        ast::AstType& returnType,
        ParseError& outError)
    {
        if (!ExpectAndConsume({TokenType::Syntax}, {"("}, outError))
        {
            return false;
        }

        auto inverse = m_Shader.AddNode<ast::InverseNode>();
        m_Shader.PushScope(inverse);

        int argumentsAccountedFor = 0;
        Token nextToken;
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

            switch (argType)
            {
                case ast::AstType::Mat3:
                    returnType = ast::AstType::Mat3;
                    argumentsAccountedFor++;
                    break;
                case ast::AstType::Mat4:
                    returnType = ast::AstType::Mat4;
                    argumentsAccountedFor++;
                    break;
                default:
                    outError = {
                    token.line, token.pos,
                    std::format("Unexpected type {}", ast::TypeUtil::TypeToGlsl(argType))
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

        if (argumentsAccountedFor != 1)
        {
            outError = {
                nextToken.line, nextToken.pos, "Too many arguments for function call inverse"
            };
            return false;
        }

        m_Shader.PopScope();

        return true;
    }

    bool Parser::ProcessMinFunc(const Token& token,
        ast::AstType& returnType,
        ParseError& outError)
    {
        if (!ExpectAndConsume({TokenType::Syntax}, {"("}, outError))
        {
            return false;
        }

        auto min = m_Shader.AddNode<ast::MinNode>();
        m_Shader.PushScope(min);

        int argumentsAccountedFor = 0;
        Token nextToken;
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

            switch (argType)
            {
                case ast::AstType::Float:
                    returnType = ast::AstType::Float;
                    argumentsAccountedFor++;
                    break;
                default:
                    outError = {
                    token.line, token.pos,
                    std::format("Unexpected type {}", ast::TypeUtil::TypeToGlsl(argType))
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
                nextToken.line, nextToken.pos, "Too many arguments for function call min"
            };
            return false;
        }

        m_Shader.PopScope();

        return true;
    }

    bool Parser::ProcessMaxFunc(const Token& token,
        ast::AstType& returnType,
        ParseError& outError)
    {
        if (!ExpectAndConsume({TokenType::Syntax}, {"("}, outError))
        {
            return false;
        }

        auto max = m_Shader.AddNode<ast::MaxNode>();
        m_Shader.PushScope(max);

        int argumentsAccountedFor = 0;
        Token nextToken;
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

            switch (argType)
            {
                case ast::AstType::Float:
                    returnType = ast::AstType::Float;
                    argumentsAccountedFor++;
                    break;
                default:
                    outError = {
                    token.line, token.pos,
                    std::format("Unexpected type {}", ast::TypeUtil::TypeToGlsl(argType))
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
                nextToken.line, nextToken.pos, "Too many arguments for function call max"
            };
            return false;
        }

        m_Shader.PopScope();

        return true;
    }
}
