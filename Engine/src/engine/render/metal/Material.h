#pragma once
#include "engine/render/render_fwd.h"

#if METAL_RENDERER

#include "engine/asset/texture/Texture.h"
#include "engine/render/Material.h"

namespace se::render::metal
{
    class Material : public render::Material
    {
    public:
        Material(const std::vector<std::shared_ptr<asset::Shader>>& vertShaders,
                 const std::vector<std::shared_ptr<asset::Shader>>& fragShaders);
        void Bind(const render::VertexBuffer&) override;
        void CreatePlatformResources(const render::VertexBuffer& vb) override;
        void DestroyPlatformResources() override;
    private:
        void SetUniformInternal(const std::string& name, asset::shader::ast::AstType::Type type, int count, const void* value) override;
    };
}
#endif