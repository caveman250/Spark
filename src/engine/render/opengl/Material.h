#pragma once
#include <Gl/glew.h>

#include "engine/render/Material.h"

namespace se::render::opengl
{
    class Material : public render::Material
    {
    public:
        Material(const std::string& vertPath, const std::string& fragPath);
        void Bind() override;
        void CreatePlatformResources() override;
    private:
        GLuint m_CompiledProgram = GL_INVALID_VALUE;
    };
}
