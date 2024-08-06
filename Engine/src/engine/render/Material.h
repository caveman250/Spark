#pragma once
#include "RenderState.h"
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

        virtual void Bind();
        virtual void CreatePlatformResources(const VertexBuffer& vb) = 0;
        void SetRenderState(const RenderState& state);
        void SetShaderSettings(const ShaderSettings& settings);

        virtual void SetUniform(const std::string& name, asset::shader::ast::AstType::Type type, const void* value) = 0;
    protected:
        Material(const std::vector<std::shared_ptr<asset::Shader>>& vertShaders,
                const std::vector<std::shared_ptr<asset::Shader>>& fragShaders);
        std::vector<std::shared_ptr<asset::Shader>> m_VertShaders;
        std::vector<std::shared_ptr<asset::Shader>> m_FragShaders;
        RenderState m_RenderState;
        ShaderSettings m_ShaderSettings; // ignored after platform resources have been created.
    };
}
