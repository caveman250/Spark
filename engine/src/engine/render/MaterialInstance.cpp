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
            if (m_PlatformResources && material->GetCachedLightSetup() != m_CachedLightSetup)
            {
                m_PlatformResources->DestroyPlatformResources();
                m_PlatformResources = nullptr;
            }
            m_CachedLightSetup = material->GetCachedLightSetup();

            EASY_BLOCK("Apply Lights");

            const auto& lightSetup = Renderer::Get<Renderer>()->GetLightSetup();
            std::vector<math::Vec3> dir;
            dir.resize(lightSetup.dirLights.size());
            std::ranges::transform(lightSetup.dirLights, dir.begin(), [](const DirLight& light){ return light.dir; });

            std::vector<math::Vec3> color;
            color.resize(lightSetup.dirLights.size());
            std::ranges::transform(lightSetup.dirLights, color.begin(), [](const DirLight& light){ return light.color; });

            SetUniform("dirLights", static_cast<int>(dir.size()), dir.data(), true);
            SetUniform("dirLightColors", static_cast<int>(color.size()), color.data(), true);

            std::vector<math::Vec3> pos;
            pos.resize(lightSetup.pointLights.size());
            std::ranges::transform(lightSetup.pointLights, pos.begin(), [](const PointLight& light){ return light.pos; });

            color.clear();
            color.resize(lightSetup.pointLights.size());
            std::ranges::transform(lightSetup.pointLights, color.begin(), [](const PointLight& light){ return light.color; });

            std::vector<float> power;
            power.resize(lightSetup.pointLights.size());
            std::ranges::transform(lightSetup.pointLights, power.begin(), [](const PointLight& light){ return light.power; });

            SetUniform("pointLightPos", static_cast<int>(pos.size()), pos.data(), true);
            SetUniform("pointLightColors", static_cast<int>(color.size()), color.data(), true);
            SetUniform("pointLightPowers", static_cast<int>(power.size()), power.data(), true);
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
        if (m_MaterialHandle)
        {
            return m_MaterialHandle;
        }

        return m_Material.IsSet() ? m_Material.GetAsset() : m_MaterialHandle;
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

    MaterialInstance::MaterialInstance(const MaterialInstance& other)
        : Asset(other)
        , m_Material(other.m_Material)
        , m_MaterialHandle(other.m_MaterialHandle)
        , m_UniformStorage(other.m_UniformStorage)
    {
    }
}


