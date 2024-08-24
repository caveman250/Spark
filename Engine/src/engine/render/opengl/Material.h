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
        void Bind(const VertexBuffer&) override;
        void CreatePlatformResources(const VertexBuffer& vb) override;
        void DestroyPlatformResources() override;

        GLuint GetProgramID() { return m_CompiledProgram; }
    private:
        void SetUniformInternal(const std::string& name, asset::shader::ast::AstType::Type type, int count, const void* value) override;

        std::vector<std::pair<std::string, std::shared_ptr<TextureResource>>> m_Textures;
        GLuint m_CompiledProgram = GL_INVALID_VALUE;
    };
}
