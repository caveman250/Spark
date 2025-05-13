#include "MaterialInstance.h"

#include "Material.h"
#include "engine/asset/shader/ast/TypeUtil.h"

#if OPENGL_RENDERER

namespace se::render
{
    std::shared_ptr<MaterialInstance> MaterialInstance::CreateMaterialInstance(const std::shared_ptr<render::Material>& material)
    {
        SPARK_ASSERT(material);
        return std::make_shared<opengl::MaterialInstance>(material);
    }
}

namespace se::render::opengl
{
    MaterialInstance::MaterialInstance(const std::shared_ptr<render::Material> &material)
        : render::MaterialInstance(material)
    {

    }

    void MaterialInstance::Bind(const VertexBuffer& vb)
    {
        render::MaterialInstance::Bind(vb);

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

            m_Textures[i].second->Bind(i);
        }
    }

    void MaterialInstance::CreatePlatformResources()
    {
        auto material = static_cast<Material*>(m_Material.get());

        m_CompiledProgram = glCreateProgram();
        glAttachShader(m_CompiledProgram, material->GetVertexShader());
        GL_CHECK_ERROR()
        glAttachShader(m_CompiledProgram, material->GetFragmentShader());
        GL_CHECK_ERROR()
        glLinkProgram(m_CompiledProgram);
        GL_CHECK_ERROR()

        // Check the program
        GLint Result = GL_FALSE;
        int InfoLogLength;
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

        glDetachShader(m_CompiledProgram, material->GetVertexShader());
        GL_CHECK_ERROR()
        glDetachShader(m_CompiledProgram, material->GetFragmentShader());
        GL_CHECK_ERROR()

        render::MaterialInstance::CreatePlatformResources();
    }

    void MaterialInstance::DestroyPlatformResources()
    {
        render::MaterialInstance::DestroyPlatformResources();

        if (m_CompiledProgram != GL_INVALID_VALUE)
        {
            glDeleteProgram(m_CompiledProgram);
            GL_CHECK_ERROR()
            m_CompiledProgram = GL_INVALID_VALUE;
        }
    }

    void MaterialInstance::SetUniformInternal(const String &name,
                                              asset::shader::ast::AstType::Type type,
                                              int count,
                                              const void *value)
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
            uniformLoc = glGetUniformLocation(m_CompiledProgram, name.Data());
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
                debug::Log::Error("Material::SetUniform - Unhandled unfiorm type {}", asset::shader::ast::TypeUtil::TypeToGlsl(type));
                break;
        }
    }
}

#endif