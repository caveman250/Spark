#include "Material.h"

#if METAL_RENDERER

#include "engine/asset/shader/compiler/ShaderCompiler.h"
#include "engine/asset/shader/ast/Types.h"
#include "engine/asset/shader/ast/TypeUtil.h"
#include "TextureResource.h"

namespace se::render
{
    std::shared_ptr<Material> Material::CreateMaterial(const std::vector<std::shared_ptr<asset::Shader>>& vertShaders,
                                                        const std::vector<std::shared_ptr<asset::Shader>>& fragShaders)
    {
        return std::make_shared<metal::Material>(vertShaders, fragShaders);
    }
}

namespace se::render::metal
{
    Material::Material(const std::vector<std::shared_ptr<asset::Shader>>& vertShaders,
                        const std::vector<std::shared_ptr<asset::Shader>>& fragShaders)
        : render::Material(vertShaders, fragShaders)
    {
    }

    void Material::Bind(const render::VertexBuffer& vb)
    {
        render::Material::Bind(vb);

        // TODO
    }

    void Material::CreatePlatformResources(const render::VertexBuffer& vb)
    {
        // TODO
        render::Material::CreatePlatformResources(vb);
    }

    void Material::DestroyPlatformResources()
    {
        // TODO
        render::Material::DestroyPlatformResources();
    }

    void Material::SetUniformInternal(const std::string& name, asset::shader::ast::AstType::Type type, int count, const void* value)
    {
        // TODO
    }
}

#endif