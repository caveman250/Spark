#include "Material.h"

#if OPENGL_RENDERER

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

    void Material::Bind(const render::VertexBuffer& vb)
    {
        render::Material::Bind(vb);
        glUseProgram(m_CompiledProgram);
        GL_CHECK_ERROR()

        for (size_t i = 0; i < m_Textures.size(); ++i)
        {
            switch (i)
            {
                case 0:
                    glActiveTexture(GL_TEXTURE0);
                GL_CHECK_ERROR()
                    break;
                case 1:
                    glActiveTexture(GL_TEXTURE1);
                GL_CHECK_ERROR()
                    break;
                case 2:
                    glActiveTexture(GL_TEXTURE2);
                GL_CHECK_ERROR()
                    break;
                case 3:
                    glActiveTexture(GL_TEXTURE3);
                GL_CHECK_ERROR()
                    break;
                case 4:
                    glActiveTexture(GL_TEXTURE4);
                GL_CHECK_ERROR()
                    break;
                case 5:
                    glActiveTexture(GL_TEXTURE5);
                GL_CHECK_ERROR()
                    break;
                case 6:
                    glActiveTexture(GL_TEXTURE6);
                GL_CHECK_ERROR()
                    break;
                case 7:
                    glActiveTexture(GL_TEXTURE7);
                GL_CHECK_ERROR()
                    break;
                case 8:
                    glActiveTexture(GL_TEXTURE8);
                GL_CHECK_ERROR()
                    break;
                case 9:
                    glActiveTexture(GL_TEXTURE9);
                GL_CHECK_ERROR()
                    break;
                case 10:
                    glActiveTexture(GL_TEXTURE10);
                GL_CHECK_ERROR()
                    break;
            }

            m_Textures[i].second->Bind();
        }
    }

    void Material::CreatePlatformResources(const render::VertexBuffer& vb)
    {
        SPARK_ASSERT(m_CompiledProgram == GL_INVALID_VALUE);

        std::optional<std::string> vert = asset::shader::ShaderCompiler::GeneratePlatformShader(m_VertShaders, m_ShaderSettings, vb, m_Textures);
        std::optional<std::string> frag = asset::shader::ShaderCompiler::GeneratePlatformShader(m_FragShaders, m_ShaderSettings, vb, m_Textures);

        if (!vert.has_value() || !frag.has_value())
        {
            return;
        }
        else
        {
            //debug::Log::Info("Result Vert Shader:\n {}", vert.value());
            //debug::Log::Info("Result Frag Shader:\n {}", frag.value());
        }


        // Create the shaders
        GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
        GL_CHECK_ERROR()
        GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
        GL_CHECK_ERROR()

        char const *VertexSourcePointer = vert.value().c_str();
        glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
        GL_CHECK_ERROR()
        glCompileShader(VertexShaderID);
        GL_CHECK_ERROR()

        GLint Result = GL_FALSE;
        int InfoLogLength;
        glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
        GL_CHECK_ERROR()
        glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        GL_CHECK_ERROR()
        if (InfoLogLength > 0)
        {
            std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
            glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
            GL_CHECK_ERROR()
            printf("%s\n", &VertexShaderErrorMessage[0]);
        }

        // Compile Fragment Shader
        char const *FragmentSourcePointer = frag.value().c_str();
        glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
        GL_CHECK_ERROR()
        glCompileShader(FragmentShaderID);
        GL_CHECK_ERROR()

        glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
        GL_CHECK_ERROR()
        glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        GL_CHECK_ERROR()
        if (InfoLogLength > 0)
        {
            std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
            glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
            GL_CHECK_ERROR()
            debug::Log::Error("{0}", &FragmentShaderErrorMessage[0]);
        }

        m_CompiledProgram = glCreateProgram();
        glAttachShader(m_CompiledProgram, VertexShaderID);
        GL_CHECK_ERROR()
        glAttachShader(m_CompiledProgram, FragmentShaderID);
        GL_CHECK_ERROR()
        glLinkProgram(m_CompiledProgram);
        GL_CHECK_ERROR()

        // Check the program
        glGetProgramiv(m_CompiledProgram, GL_LINK_STATUS, &Result);
        GL_CHECK_ERROR()
        glGetProgramiv(m_CompiledProgram, GL_INFO_LOG_LENGTH, &InfoLogLength);
        GL_CHECK_ERROR()
        if (InfoLogLength > 0)
        {
            std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
            glGetProgramInfoLog(m_CompiledProgram, InfoLogLength, NULL, &ProgramErrorMessage[0]);
            GL_CHECK_ERROR()
            debug::Log::Error("{0}", &ProgramErrorMessage[0]);
        }

        glDetachShader(m_CompiledProgram, VertexShaderID);
        GL_CHECK_ERROR()
        glDetachShader(m_CompiledProgram, FragmentShaderID);
        GL_CHECK_ERROR()

        glDeleteShader(VertexShaderID);
        GL_CHECK_ERROR()
        glDeleteShader(FragmentShaderID);
        GL_CHECK_ERROR()

        render::Material::CreatePlatformResources(vb);
    }

    void Material::DestroyPlatformResources()
    {
        if (m_CompiledProgram != GL_INVALID_VALUE)
        {
            glDeleteProgram(m_CompiledProgram);
            GL_CHECK_ERROR()
            m_CompiledProgram = GL_INVALID_VALUE;
        }
        render::Material::DestroyPlatformResources();
    }

    void Material::SetUniformInternal(const std::string& name, asset::shader::ast::AstType::Type type, int count, const void* value)
    {
        if (!m_PlatformResourcesCreated)
        {
            return;
        }


        glUseProgram(m_CompiledProgram);
        GL_CHECK_ERROR()
        GLuint uniformLoc = {};
        if (type != asset::shader::ast::AstType::Sampler2D)
        {
            uniformLoc = glGetUniformLocation(m_CompiledProgram, name.c_str());
            GL_CHECK_ERROR()
        }

        switch (type)
        {
        case asset::shader::ast::AstType::Int:
            glUniform1iv(uniformLoc, count, static_cast<const int*>(value));
            GL_CHECK_ERROR()
            break;
        case asset::shader::ast::AstType::Float:
            glUniform1fv(uniformLoc, count, static_cast<const float*>(value));
            GL_CHECK_ERROR()
            break;
        case asset::shader::ast::AstType::Vec2:
            glUniform2fv(uniformLoc, count, static_cast<const float*>(value));
            GL_CHECK_ERROR()
            break;
        case asset::shader::ast::AstType::Vec3:
            glUniform3fv(uniformLoc, count, static_cast<const float*>(value));
            GL_CHECK_ERROR()
            break;
        case asset::shader::ast::AstType::Vec4:
            glUniform4fv(uniformLoc, count, static_cast<const float*>(value));
            GL_CHECK_ERROR()
            break;
        case asset::shader::ast::AstType::Mat3:
            glUniformMatrix3fv(uniformLoc, count, false, static_cast<const float*>(value));
            GL_CHECK_ERROR()
            break;
        case asset::shader::ast::AstType::Mat4:
            glUniformMatrix4fv(uniformLoc, count, false, static_cast<const float*>(value));
            GL_CHECK_ERROR()
            break;
        case asset::shader::ast::AstType::Sampler2D:
        {
            SPARK_ASSERT(count == 1, "Setting arrays of texture uniforms not supported.");
            const auto& texture = *reinterpret_cast<const std::shared_ptr<asset::Texture>*>(value);
            const auto& platformResource = texture->GetPlatformResource();
            auto it = std::ranges::find_if(m_Textures, [name](const auto& kvp){ return kvp.first == name; });
            if (it != m_Textures.end())
            {
                it->second = platformResource;
            }
            else
            {
                m_Textures.push_back(std::make_pair(name, platformResource));
            }
            break;
        }
        case asset::shader::ast::AstType::Void:
        case asset::shader::ast::AstType::Invalid:
            debug::Log::Error("Material::SetUniform - Unhandled unfiorm type {}", asset::shader::ast::TypeUtil::GetTypeGlsl(type));
            break;
        }
    }
}

#endif