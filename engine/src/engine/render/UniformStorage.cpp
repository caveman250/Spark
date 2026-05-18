#include "UniformStorage.h"
#include "MaterialInstance.h"
#include "MaterialInstancePlatformResources.h"

namespace se
{
    render::UniformStorage::~UniformStorage()
    {
        for (const auto& value: m_Storage | std::views::values)
        {
            delete value;
        }
        m_Storage.clear();
    }

    render::UniformStorage::UniformStorage(const UniformStorage& other)
        : m_Stale(true)
    {
        for (const auto& [key, value] : other.m_Storage)
        {
            m_Storage.insert(std::make_pair(key, value->Copy()));
        }
    }

    void render::UniformStorage::Apply(MaterialInstance* material)
    {
        for (const auto& [name, value] : m_Storage)
        {
            material->GetPlatformResources()->SetUniformInternal(name, value->GetShaderType(), static_cast<int>(value->GetValueCount()), value->GetValue(), material->GetMaterial());
        }

        m_Stale = false;
    }

    void render::UniformStorage::ApplyTo(UniformStorage& other) const
    {
        for (const auto& [name, value] : m_Storage)
        {
            if (other.m_Storage.contains(name))
            {
                other.m_Storage.at(name)->SetValue(value->GetValue(), static_cast<int>(value->GetValueCount()), value->internal);
            }
            else
            {
                other.m_Storage.insert(std::make_pair(name, value->Copy()));
            }
        }
    }
}
