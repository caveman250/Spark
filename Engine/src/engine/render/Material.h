#pragma once
#include "RenderState.h"

namespace se::shader::ast
{
    enum class Type;
}

namespace se::render
{
    class Material
    {
    public:
        static std::shared_ptr<Material> CreateMaterial(const std::vector<std::string>& filePaths, const std::vector<std::string>& fragPaths);

        virtual ~Material() = default;

        virtual void Bind();
        virtual void CreatePlatformResources() = 0;
        void SetRenderState(const RenderState& state);

        virtual void SetUniform(const std::string& name, shader::ast::Type type, const void* value) = 0;
    protected:
        Material(const std::vector<std::string>& vertPaths, const std::vector<std::string>& fragPaths);
        std::vector<std::string> m_VertShaderPaths;
        std::vector<std::string> m_FragShaderPaths;
        RenderState m_RenderState;
    };
}
