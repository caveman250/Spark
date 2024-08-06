#pragma once

#include "engine/asset/texture/Texture.h"
#include "engine/render/Material.h"
#include "GL_fwd.h"
#include "engine/asset/shader/ShaderSettings.h"

namespace se::render::opengl
{
    class Material : public render::Material
    {
    public:
        Material(const std::vector<std::shared_ptr<asset::Shader>>& vertShaders,
                 const std::vector<std::shared_ptr<asset::Shader>>& fragShaders);
        void Bind() override;
        void CreatePlatformResources(const render::VertexBuffer& vb) override;
        void SetUniform(const std::string& name, asset::shader::ast::AstType::Type type, const void* value) override;

        GLuint GetProgramID() { return m_CompiledProgram; }
    private:
        std::vector<std::shared_ptr<TextureResource>> m_Textures;
        GLuint m_CompiledProgram = GL_INVALID_VALUE;
    };
}
