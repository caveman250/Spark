#include "MaterialInstance.h"
#include "Material.h"
#include "MaterialInstancePlatformResources.h"
#include "Renderer.h"
#include "easy/profiler.h"

namespace se::render
{
    MaterialInstance::~MaterialInstance()
    {
        if (m_PlatformResources)
        {
            m_PlatformResources->DestroyPlatformResources();
        }
    }

    void MaterialInstance::Bind(const VertexBuffer& vb)
    {
        EASY_FUNCTION();

        const auto& material = GetMaterial();
        material->Bind(vb);

        if (material->GetRenderState().lit)
        {
            EASY_BLOCK("Apply Lights");

            const auto& lightSetup = Renderer::Get<Renderer>()->GetLightSetup();
            std::vector<math::Vec3> pos;
            pos.resize(lightSetup.pointLights.size());
            std::ranges::transform(lightSetup.pointLights, pos.begin(), [](const PointLight& light){ return light.pos; });

            std::vector<math::Vec3> color;
            color.resize(lightSetup.pointLights.size());
            std::ranges::transform(lightSetup.pointLights, color.begin(), [](const PointLight& light){ return light.color; });

            SetUniform("lightPos", static_cast<int>(pos.size()), pos.data());
            SetUniform("lightColor", static_cast<int>(color.size()), color.data());
        }

        if (!m_PlatformResources)
        {
            m_PlatformResources = CreateMaterialInstancePlatformResources(material);
        }

        if (m_UniformStorage.IsStale())
        {
            EASY_BLOCK("Apply Uniforms");
            m_UniformStorage.Apply(this);
        }

        m_PlatformResources->Bind(vb);
    }

    const std::shared_ptr<Material>& MaterialInstance::GetMaterial()
    {
        if (!m_MaterialHandle && m_Material.IsSet())
        {
            m_MaterialHandle = m_Material.GetAsset();
        }
        return m_MaterialHandle;
    }

    MaterialInstance::MaterialInstance(const asset::AssetReference<Material>& material)
        : m_Material(material)
    {
        material.GetAsset()->GetUniformDefaults().ApplyTo(m_UniformStorage);
    }

    MaterialInstance::MaterialInstance(const std::shared_ptr<Material>& material)
        : m_MaterialHandle(material)
    {

    }
}


