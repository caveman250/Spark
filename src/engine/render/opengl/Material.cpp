#include "Material.h"

#include "engine/logging/Log.h"
#include "engine/shader/ShaderGenerator.h"

namespace se::render
{
    std::shared_ptr<Material> Material::CreateMaterial(const std::string &vertPath, const std::string &fragPath)
    {
        return std::make_shared<opengl::Material>(vertPath, fragPath);
    }
}

namespace se::render::opengl
{
    Material::Material(const std::string &vertPath, const std::string &fragPath)
        : render::Material(vertPath, fragPath)
    {
    }

    void Material::Bind()
    {
        glUseProgram(m_CompiledProgram);
    }

    void Material::CreatePlatformResources()
    {
        std::optional<std::string> vert = se::shader::ShaderGenerator::CompileShader(m_VertShaderPath);
        std::optional<std::string> frag = se::shader::ShaderGenerator::CompileShader(m_FragShaderPath);

        if (!vert.has_value() || !frag.has_value())
        {
            logging::Log::Error("Failed to load shaders vert: %s, frag: %s", m_VertShaderPath, m_FragShaderPath);
            return;
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
            logging::Log::Error("%s", &FragmentShaderErrorMessage[0]);
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
            logging::Log::Error("%s", &ProgramErrorMessage[0]);
        }

        glDetachShader(m_CompiledProgram, VertexShaderID);
        glDetachShader(m_CompiledProgram, FragmentShaderID);

        glDeleteShader(VertexShaderID);
        glDeleteShader(FragmentShaderID);
    }
}
