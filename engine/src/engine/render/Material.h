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

        bool IsDataAsset() const override { return true; }

        virtual void Bind(const VertexBuffer& vb);
        virtual void CreatePlatformResources(const VertexBuffer& vb);
        virtual void DestroyPlatformResources();
        void SetRenderState(const RenderState& state);
        ShaderSettings& GetShaderSettings();
        const RenderState& GetRenderState() const { return m_RenderState; }
        const std::shared_ptr<MaterialPlatformResources>& GetPlatformResources() const { return m_PlatformResources; }

        UniformStorage& GetUniformDefaults();

        const std::vector<asset::AssetReference<asset::Shader>>& GetVertShaders() const { return m_VertShaders; }
        const std::vector<asset::AssetReference<asset::Shader>>& GetFragShaders() const { return m_FragShaders; }
        const std::map<std::string, asset::SerializedVariable>& GetVertUniforms() const { return m_VertUniforms; }
        const std::map<std::string, asset::SerializedVariable>& GetFragUniforms() const { return m_FragUniforms; }

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
        std::map<std::string, asset::SerializedVariable> m_VertUniforms = {};
        std::map<std::string, asset::SerializedVariable> m_FragUniforms = {};
    };
}
