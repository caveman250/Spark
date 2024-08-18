#include "UniformStorage.h"

#include "Material.h"

namespace se
{
    render::UniformStorage::~UniformStorage()
    {
        for (const auto& [name, value] : m_Storage)
        {
            delete value;
        }
    }

    void render::UniformStorage::Apply(Material* material)
    {
        for (const auto& [name, value] : m_Storage)
        {
            material->SetUniformInternal(name, value->type, value->valueCount, value->GetValue());
        }

        m_Stale = false;
    }
}
