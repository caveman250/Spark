#pragma once
#include <Gl/glew.h>

#include "engine/render/Material.h"

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
        GLuint m_CompiledProgram = GL_INVALID_VALUE;
    };
}
