#pragma once
#include "RenderState.h"
#include "engine/asset/shader/Shader.h"
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
        static std::shared_ptr<Material> CreateMaterial(const std::vector<std::shared_ptr<asset::shader::ast::Shader>>& vertShaders,
                                                        const std::vector<std::shared_ptr<asset::shader::ast::Shader>>& fragShaders);

        virtual ~Material() = default;

        virtual void Bind();
        virtual void CreatePlatformResources(const VertexBuffer& vb) = 0;
        void SetRenderState(const RenderState& state);

        virtual void SetUniform(const std::string& name, asset::shader::ast::AstType::Type type, const void* value) = 0;
    protected:
        Material(const std::vector<std::shared_ptr<asset::shader::ast::Shader>>& vertShaders,
                const std::vector<std::shared_ptr<asset::shader::ast::Shader>>& fragShaders);
        std::vector<std::shared_ptr<asset::shader::ast::Shader>> m_VertShaders;
        std::vector<std::shared_ptr<asset::shader::ast::Shader>> m_FragShaders;
        RenderState m_RenderState;
    };
}
