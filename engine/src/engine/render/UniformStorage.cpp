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

    void render::UniformStorage::Apply(MaterialInstance* material)
    {
        for (const auto& [name, value] : m_Storage)
        {
            material->GetPlatformResources()->SetUniformInternal(name, value->GetShaderType(), value->GetValueCount(), value->GetValue(), material->GetMaterial());
        }

        m_Stale = false;
    }

    void render::UniformStorage::ApplyTo(UniformStorage& other) const
    {
        for (const auto& [name, value] : m_Storage)
        {
            if (other.m_Storage.contains(name))
            {
                other.m_Storage.at(name)->SetValue(value->GetValue(), value->GetValueCount(), value->internal);
            }
            else
            {
                other.m_Storage.insert(std::make_pair(name, value->Copy()));
            }
        }
    }
}
