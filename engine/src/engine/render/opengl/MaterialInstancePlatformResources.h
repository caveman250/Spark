#pragma once


#include "engine/render/MaterialInstancePlatformResources.h"
#include "engine/render/render_fwd.h"

#if OPENGL_RENDERER

#include "GL/glew.h"

namespace se::render
{
    class Material;
    class TextureResource;
}

namespace se::render::opengl
{
    class MaterialInstancePlatformResources : public render::MaterialInstancePlatformResources
    {
    public:
        void Bind(const VertexBuffer& vb) override;
        void CreatePlatformResources(const std::shared_ptr<Material>& material) override;
        void DestroyPlatformResources() override;
        void SetUniformInternal(const std::string& name,
            asset::shader::ast::AstType type,
            int count,
            const void* value,
            const std::shared_ptr<Material>& material) override;

        GLuint GetProgramID() { return m_CompiledProgram; }

    private:
        std::vector<std::pair<std::string, std::shared_ptr<render::TextureResource>>> m_Textures;
        GLuint m_CompiledProgram = GL_INVALID_VALUE;
    };
}

#endif