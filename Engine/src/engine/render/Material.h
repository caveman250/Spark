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

        const RenderState& GetRenderState() const { return m_RenderState; }

        const std::map<String, asset::shader::ast::Variable>& GetVertUniforms() const { return m_VertUniforms; }
        const std::map<String, asset::shader::ast::Variable>& GetFragUniforms() const { return m_FragUniforms; }

    protected:
        virtual void ApplyDepthStencil(DepthCompare::Type comp, StencilFunc::Type src, uint32_t writeMask, uint32_t readMask) = 0;
        virtual void ApplyBlendMode(BlendMode::Type src, BlendMode::Type dest) = 0;
        Material(const std::vector<std::shared_ptr<asset::Shader>>& vertShaders,
                const std::vector<std::shared_ptr<asset::Shader>>& fragShaders);

        std::vector<std::shared_ptr<asset::Shader>> m_VertShaders;
        std::vector<std::shared_ptr<asset::Shader>> m_FragShaders;
        RenderState m_RenderState;
        ShaderSettings m_ShaderSettings; // ignored after platform resources have been created.
        LightSetup m_CachedLightSetup;
        bool m_PlatformResourcesCreated = false;

        std::map<String, asset::shader::ast::Variable> m_VertUniforms;
        std::map<String, asset::shader::ast::Variable> m_FragUniforms;
    };
}
