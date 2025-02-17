#include "Material.h"

#include <engine/asset/shader/ast/NameGenerator.h>
#include <engine/asset/shader/ast/ShaderCompileContext.h>

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
        asset::shader::ast::ShaderCompileContext context = { nullptr, nullptr, nullptr, asset::shader::ast::NameGenerator::GetName() , {}, {} };

        auto fragUniforms = asset::shader::ShaderCompiler::GatherUsedUniforms(m_FragShaders);
        context.fragmentShaderUniforms = fragUniforms;
        std::optional<std::string> vert = asset::shader::ShaderCompiler::GeneratePlatformShader(m_VertShaders, m_ShaderSettings, vb, context);
        std::optional<std::string> frag = asset::shader::ShaderCompiler::GeneratePlatformShader(m_FragShaders, m_ShaderSettings, vb, context);

        if (!vert.has_value() || !frag.has_value())
        {
            return;
        }

        debug::Log::Info("Result Frag Shader:\n {}", frag.value());

        auto device = Renderer::Get<metal::MetalRenderer>()->GetDevice();

        NS::Error* error = nullptr;
        MTL::Library* vertLibrary = device->newLibrary( NS::String::string(vert->c_str(), NS::UTF8StringEncoding), nullptr, &error );
        if (!vertLibrary)
        {
            debug::Log::Error("Failed to compile shader!\nShader Text:\n{}\nError(s):\n{}", vert->c_str(), error->localizedDescription()->utf8String());
            SPARK_ASSERT(false);
        }

        MTL::Library* fragLibrary = device->newLibrary( NS::String::string(frag->c_str(), NS::UTF8StringEncoding), nullptr, &error );
        if (!fragLibrary)
        {
            debug::Log::Error("Failed to compile shader!\nShader Text:\n{}\nError(s):\n{}", frag->c_str(), error->localizedDescription()->utf8String());
            SPARK_ASSERT(false);
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