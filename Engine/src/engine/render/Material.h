#pragma once
#include "LightSetup.h"
#include "RenderState.h"
#include "UniformStorage.h"
#include "engine/asset/shader/Shader.h"
#include "engine/asset/shader/ShaderSettings.h"
#include "engine/asset/shader/ast/Types.h"

namespace se::asset::shader::ast
{
    enum class Type;
}

namespace se::render
{
    class VertexBuffer;

    class Material
    {
    public:
        static std::shared_ptr<Material> CreateMaterial(const std::vector<std::shared_ptr<asset::Shader>>& vertShaders,
                                                        const std::vector<std::shared_ptr<asset::Shader>>& fragShaders);

        virtual ~Material() = default;

        virtual void Bind(const VertexBuffer& vb);
        virtual void CreatePlatformResources(const VertexBuffer& vb);
        virtual void DestroyPlatformResources();
        void SetRenderState(const RenderState& state);
        ShaderSettings& GetShaderSettings();

        template <typename T>
        void SetUniform(const std::string& name, asset::shader::ast::AstType::Type type, int count, const T* value);
    protected:
        virtual void SetUniformInternal(const std::string& name, asset::shader::ast::AstType::Type type, int count, const void* value) = 0;
        Material(const std::vector<std::shared_ptr<asset::Shader>>& vertShaders,
                const std::vector<std::shared_ptr<asset::Shader>>& fragShaders);
        std::vector<std::shared_ptr<asset::Shader>> m_VertShaders;
        std::vector<std::shared_ptr<asset::Shader>> m_FragShaders;
        UniformStorage m_UniformStorage;
        RenderState m_RenderState;
        ShaderSettings m_ShaderSettings; // ignored after platform resources have been created.
        LightSetup m_CachedLightSetup;
        bool m_PlatformResourcesCreated = false;

        friend class UniformStorage;
    };

    template <typename T>
    void Material::SetUniform(const std::string& name, asset::shader::ast::AstType::Type type, int count, const T* value)
    {
        m_UniformStorage.SetValue(name, type, count, value);
    }
}
