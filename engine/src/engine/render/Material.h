#pragma once
#include "LightSetup.h"
#include "MaterialPlatformResources.h"
#include "RenderState.h"
#include "UniformStorage.h"
#include "engine/asset/AssetReference.h"
#include "engine/asset/shader/Shader.h"
#include "engine/asset/shader/ShaderSettings.h"

namespace se::asset::shader::ast
{
    enum class Type;
}

namespace se::render
{
    class VertexBuffer;

    class Material : public asset::Asset
    {
        SPARK_CLASS()
    public:
        Material() = default;
        Material(const std::vector<asset::AssetReference<asset::Shader>>& vertShaders,
                const std::vector<asset::AssetReference<asset::Shader>>& fragShaders);

        static std::shared_ptr<MaterialPlatformResources> CreateMaterialPlatformResources();

        ~Material() override = default;

        virtual void Bind(const VertexBuffer& vb);
        virtual void CreatePlatformResources(const VertexBuffer& vb);
        virtual void DestroyPlatformResources();
        void SetRenderState(const RenderState& state);
        ShaderSettings& GetShaderSettings();
        const RenderState& GetRenderState() const { return m_RenderState; }
        const std::shared_ptr<MaterialPlatformResources>& GetPlatformResources() const { return m_PlatformResources; }

        template <typename T>
        void SetUniform(const std::string& name, asset::shader::ast::AstType type, int count, const T* value);

        const UniformStorage& GetUniformDefaults();

        const std::map<std::string, asset::shader::ast::Variable>& GetVertUniforms() const { return m_VertUniforms; }
        const std::map<std::string, asset::shader::ast::Variable>& GetFragUniforms() const { return m_FragUniforms; }

    protected:
        SPARK_MEMBER(Serialized)
        std::vector<asset::AssetReference<asset::Shader>> m_VertShaders = {};

        SPARK_MEMBER(Serialized)
        std::vector<asset::AssetReference<asset::Shader>> m_FragShaders = {};

        SPARK_MEMBER(Serialized)
        RenderState m_RenderState = {};

        SPARK_MEMBER(Serialized)
        ShaderSettings m_ShaderSettings = {}; // ignored after platform resources have been created.

        SPARK_MEMBER(Serialized)
        UniformStorage m_UniformDefaults = {};

        LightSetup m_CachedLightSetup = {};
        bool m_PlatformResourcesCreated = false;
        std::shared_ptr<MaterialPlatformResources> m_PlatformResources = nullptr;
        std::map<std::string, asset::shader::ast::Variable> m_VertUniforms = {};
        std::map<std::string, asset::shader::ast::Variable> m_FragUniforms = {};
    };

    template <typename T>
    void Material::SetUniform(const std::string& name, asset::shader::ast::AstType type, int count, const T* value)
    {
        SPARK_ASSERT(type != asset::shader::ast::AstType::Sampler2D, "Sampler2D not supported in uniform defaults. use Sampler2DReference instead.");
        m_UniformDefaults.SetValue(name, type, count, value);
    }
}
