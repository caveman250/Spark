#pragma once

#include "engine/render/MaterialInstance.h"

#if OPENGL_RENDERER

namespace se::render::opengl
{
    class MaterialInstance : public render::MaterialInstance
    {
    public:
        explicit MaterialInstance(const std::shared_ptr<render::Material>& material);
        void Bind(const render::VertexBuffer&) override;
        void CreatePlatformResources() override;
        void DestroyPlatformResources() override;

        GLuint GetProgramID() { return m_CompiledProgram; }

    private:
        void SetUniformInternal(const std::string& name,
                                asset::shader::ast::AstType type,
                                int count,
                                const void* value) override;

        std::vector<std::pair<std::string, std::shared_ptr<render::TextureResource>>> m_Textures;
        GLuint m_CompiledProgram = GL_INVALID_VALUE;
    };
}

#endif
