#pragma once

namespace se::shader::ast
{
    enum class Type;
}

namespace se::render
{
    class Material
    {
    public:
        static std::shared_ptr<Material> CreateMaterial(const std::string& vertPath, const std::string& fragPath);

        virtual void Bind() = 0;
        virtual void CreatePlatformResources() = 0;

        virtual void SetUniform(const std::string& name, shader::ast::Type type, const void* value) = 0;
    protected:
        Material(const std::string& vertPath, const std::string& fragPath);
        std::string m_VertShaderPath;
        std::string m_FragShaderPath;
    };
}
