#include "MaterialInstance.h"
#include "Material.h"
#include "Renderer.h"

namespace se::render
{
    void MaterialInstance::Bind(const VertexBuffer& vb)
    {
        m_Material->Bind(vb);

        if (m_Material->GetRenderState().lit)
        {
            const auto& lightSetup = Renderer::Get<Renderer>()->GetLightSetup();
            // TODO improve shader parser so i can just pass an array of structs
            std::vector<math::Vec3> pos;
            pos.resize(lightSetup.pointLights.size());
            std::transform(lightSetup.pointLights.begin(), lightSetup.pointLights.end(), pos.begin(), [](const PointLight& light){ return light.pos; });

            std::vector<math::Vec3> color;
            color.resize(lightSetup.pointLights.size());
            std::transform(lightSetup.pointLights.begin(), lightSetup.pointLights.end(), color.begin(), [](const PointLight& light){ return light.color; });

            SetUniform("lightPos", asset::shader::ast::AstType::Vec3, static_cast<int>(pos.size()), &pos[0]);
            SetUniform("lightColor", asset::shader::ast::AstType::Vec3, static_cast<int>(color.size()), &color[0]);
        }

        if (!m_PlatformResourcesCreated)
        {
            CreatePlatformResources();
            m_PlatformResourcesCreated = true;
        }

        if (m_UniformStorage.IsStale())
        {
            m_UniformStorage.Apply(this);
        }
    }

    void MaterialInstance::CreatePlatformResources()
    {
        m_PlatformResourcesCreated = true;
    }

    void MaterialInstance::DestroyPlatformResources()
    {
        m_PlatformResourcesCreated = false;
    }

    MaterialInstance::MaterialInstance(const std::shared_ptr<Material>& material)
        : m_Material(material)
    {

    }
}


