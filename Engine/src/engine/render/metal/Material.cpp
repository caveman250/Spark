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
        std::vector<std::pair<std::string, std::shared_ptr<render::TextureResource>>> empty = {}; // TODO
        std::optional<std::string> vert = asset::shader::ShaderCompiler::GeneratePlatformShader(m_VertShaders, m_ShaderSettings, vb, empty);
        std::optional<std::string> frag = asset::shader::ShaderCompiler::GeneratePlatformShader(m_FragShaders, m_ShaderSettings, vb, empty);

        if (!vert.has_value() || !frag.has_value())
        {
            return;
        }

        debug::Log::Info("Result Vert Shader:\n {}", vert.value());
        //debug::Log::Info("Result Frag Shader:\n {}", frag.value());

        auto device = Renderer::Get<metal::MetalRenderer>()->GetDevice();

        NS::Error* pError = nullptr;
        MTL::Library* pLibrary = device->newLibrary( NS::String::string(vert->c_str(), NS::UTF8StringEncoding), nullptr, &pError );
        if ( !pLibrary )
        {
            debug::Log::Error( "{}", pError->localizedDescription()->utf8String() );
            assert( false );
        }

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