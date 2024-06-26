#pragma once

#include "engine/asset/texture/Texture.h"
#include "engine/render/Material.h"
#include "GL_fwd.h"

namespace se::render::opengl
{
    class Material : public render::Material
    {
    public:
        Material(const std::vector<std::string>& vertPaths, const std::vector<std::string>& fragPaths);
        void Bind() override;
        void CreatePlatformResources() override;
        void SetUniform(const std::string& name, shader::ast::Type type, const void* value) override;

        GLuint GetProgramID() { return m_CompiledProgram; }
    private:
        std::vector<std::shared_ptr<TextureResource>> m_Textures;
        GLuint m_CompiledProgram = GL_INVALID_VALUE;
    };
}
