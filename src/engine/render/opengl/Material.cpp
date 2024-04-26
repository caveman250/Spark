#include "Material.h"

#include "GL_fwd.h"
#include "engine/shader/ShaderGenerator.h"
#include "engine/shader/ast/Types.h"
#include "engine/shader/ast/TypeUtil.h"

namespace se::render
{
    std::shared_ptr<Material> Material::CreateMaterial(const std::vector<std::string>& vertPaths, const std::vector<std::string>& fragPaths)
    {
        return std::make_shared<opengl::Material>(vertPaths, fragPaths);
    }
}

namespace se::render::opengl
{
    Material::Material(const std::vector<std::string>& vertPaths, const std::vector<std::string>& fragPaths)
        : render::Material(vertPaths, fragPaths)
    {
    }

    void Material::Bind()
    {
        render::Material::Bind();
        glUseProgram(m_CompiledProgram);

    }

    void Material::CreatePlatformResources()
    {
        std::optional<std::string> vert = se::shader::ShaderGenerator::CompileShader(m_VertShaderPaths);
        std::optional<std::string> frag = se::shader::ShaderGenerator::CompileShader(m_FragShaderPaths);

        if (!vert.has_value() || !frag.has_value())
        {
            return;
        }
        else
        {
            debug::Log::Info("Result Vert Shader:\n {}", vert.value());
            debug::Log::Info("Result Frag Shader:\n {}", frag.value());
        }

        // Create the shaders
        GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
        GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

        char const *VertexSourcePointer = vert.value().c_str();
        glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
        glCompileShader(VertexShaderID);

        GLint Result = GL_FALSE;
        int InfoLogLength;
        glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
        glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if (InfoLogLength > 0)
        {
            std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
            glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
            printf("%s\n", &VertexShaderErrorMessage[0]);
        }

        // Compile Fragment Shader
        char const *FragmentSourcePointer = frag.value().c_str();
        glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
        glCompileShader(FragmentShaderID);

        glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
        glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if (InfoLogLength > 0)
        {
            std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
            glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
            debug::Log::Error("{0}", &FragmentShaderErrorMessage[0]);
        }

        m_CompiledProgram = glCreateProgram();
        glAttachShader(m_CompiledProgram, VertexShaderID);
        glAttachShader(m_CompiledProgram, FragmentShaderID);
        glLinkProgram(m_CompiledProgram);

        // Check the program
        glGetProgramiv(m_CompiledProgram, GL_LINK_STATUS, &Result);
        glGetProgramiv(m_CompiledProgram, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if (InfoLogLength > 0)
        {
            std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
            glGetProgramInfoLog(m_CompiledProgram, InfoLogLength, NULL, &ProgramErrorMessage[0]);
            debug::Log::Error("{0}", &ProgramErrorMessage[0]);
        }

        glDetachShader(m_CompiledProgram, VertexShaderID);
        glDetachShader(m_CompiledProgram, FragmentShaderID);

        glDeleteShader(VertexShaderID);
        glDeleteShader(FragmentShaderID);
    }

    void Material::SetUniform(const std::string& name, shader::ast::Type type, const void* value)
    {
        Bind();
        GLuint uniformLoc = glGetUniformLocation(m_CompiledProgram, "MVP");
        GL_CHECK_ERROR()

        switch (type)
        {
        case shader::ast::Type::Float:
            glUniform1f(uniformLoc, *static_cast<const float*>(value));
            break;
        case shader::ast::Type::Vec2:
            glUniform2fv(uniformLoc, 1, static_cast<const float*>(value));
            break;
        case shader::ast::Type::Vec3:
            glUniform3fv(uniformLoc, 1, static_cast<const float*>(value));
            break;
        case shader::ast::Type::Vec4:
            glUniform4fv(uniformLoc, 1, static_cast<const float*>(value));
            break;
        case shader::ast::Type::Mat3:
            glUniformMatrix3fv(uniformLoc, 1, false, static_cast<const float*>(value));
            break;
        case shader::ast::Type::Mat4:
            glUniformMatrix4fv(uniformLoc, 1, false, static_cast<const float*>(value));
            break;
        case shader::ast::Type::Void:
        case shader::ast::Type::Invalid:
            debug::Log::Error("Material::SetUniform - Unhandled unfiorm type {}", shader::ast::TypeUtil::GetTypeGlsl(type));
            break;
        }

        GL_CHECK_ERROR()
    }
}
