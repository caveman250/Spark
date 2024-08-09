#include "Material.h"

#include "GL_fwd.h"
#include "engine/asset/shader/compiler/ShaderCompiler.h"
#include "engine/asset/shader/ast/Types.h"
#include "engine/asset/shader/ast/TypeUtil.h"
#include "TextureResource.h"

namespace se::render
{
    std::shared_ptr<Material> Material::CreateMaterial(const std::vector<std::shared_ptr<asset::Shader>>& vertShaders,
                                                        const std::vector<std::shared_ptr<asset::Shader>>& fragShaders)
    {
        return std::make_shared<opengl::Material>(vertShaders, fragShaders);
    }
}

namespace se::render::opengl
{
    Material::Material(const std::vector<std::shared_ptr<asset::Shader>>& vertShaders,
                        const std::vector<std::shared_ptr<asset::Shader>>& fragShaders)
        : render::Material(vertShaders, fragShaders)
    {
    }

    void Material::Bind()
    {
        render::Material::Bind();
        glUseProgram(m_CompiledProgram);

        for (size_t i = 0; i < m_Textures.size(); ++i)
        {
            switch (i)
            {
                case 0:
                    glActiveTexture(GL_TEXTURE0);
                    break;
                case 1:
                    glActiveTexture(GL_TEXTURE1);
                    break;
                case 2:
                    glActiveTexture(GL_TEXTURE2);
                    break;
                case 3:
                    glActiveTexture(GL_TEXTURE3);
                    break;
                case 4:
                    glActiveTexture(GL_TEXTURE4);
                    break;
                case 5:
                    glActiveTexture(GL_TEXTURE5);
                    break;
                case 6:
                    glActiveTexture(GL_TEXTURE6);
                    break;
                case 7:
                    glActiveTexture(GL_TEXTURE7);
                    break;
                case 8:
                    glActiveTexture(GL_TEXTURE8);
                    break;
                case 9:
                    glActiveTexture(GL_TEXTURE9);
                    break;
                case 10:
                    glActiveTexture(GL_TEXTURE10);
                    break;
            }

            m_Textures[i]->Bind();
        }
    }

    void Material::CreatePlatformResources(const VertexBuffer& vb)
    {
        std::optional<std::string> vert = asset::shader::ShaderCompiler::GeneratePlatformShader(m_VertShaders, m_ShaderSettings, vb);
        std::optional<std::string> frag = asset::shader::ShaderCompiler::GeneratePlatformShader(m_FragShaders, m_ShaderSettings, vb);

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

    void Material::SetUniform(const std::string& name, asset::shader::ast::AstType::Type type, int count, const void* value)
    {
        glUseProgram(m_CompiledProgram);
        GLuint uniformLoc = glGetUniformLocation(m_CompiledProgram, name.c_str());
        GL_CHECK_ERROR()

        switch (type)
        {
        case asset::shader::ast::AstType::Int:
            glUniform1iv(uniformLoc, count, static_cast<const int*>(value));
            break;
        case asset::shader::ast::AstType::Float:
            glUniform1fv(uniformLoc, count, static_cast<const float*>(value));
            break;
        case asset::shader::ast::AstType::Vec2:
            glUniform2fv(uniformLoc, count, static_cast<const float*>(value));
            break;
        case asset::shader::ast::AstType::Vec3:
            glUniform3fv(uniformLoc, count, static_cast<const float*>(value));
            break;
        case asset::shader::ast::AstType::Vec4:
            glUniform4fv(uniformLoc, count, static_cast<const float*>(value));
            break;
        case asset::shader::ast::AstType::Mat3:
            glUniformMatrix3fv(uniformLoc, count, false, static_cast<const float*>(value));
            break;
        case asset::shader::ast::AstType::Mat4:
            glUniformMatrix4fv(uniformLoc, count, false, static_cast<const float*>(value));
            break;
        case asset::shader::ast::AstType::Sampler2D:
        {
            SPARK_ASSERT(count == 1, "Setting arrays of texture uniforms not supported.");
            auto texture = reinterpret_cast<const asset::Texture *>(value);
            const auto& platformResource = texture->GetPlatformResource();
            if (!std::ranges::contains(m_Textures, platformResource))
            {
                m_Textures.push_back(platformResource);
            }
            break;
        }
        case asset::shader::ast::AstType::Void:
        case asset::shader::ast::AstType::Invalid:
            debug::Log::Error("Material::SetUniform - Unhandled unfiorm type {}", asset::shader::ast::TypeUtil::GetTypeGlsl(type));
            break;
        }

        GL_CHECK_ERROR()
    }
}
