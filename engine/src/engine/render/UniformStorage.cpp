#include "UniformStorage.h"
#include "MaterialInstance.h"
#include "engine/string/String.h"

namespace se
{
    render::UniformStorage::~UniformStorage()
    {
        for (const auto& [name, value] : m_Storage)
        {
            delete value;
        }
        m_Storage.clear();
    }

    void render::UniformStorage::Apply(MaterialInstance* material)
    {
        for (const auto& [name, value] : m_Storage)
        {
            material->SetUniformInternal(name, value->type, value->valueCount, value->GetValue());
        }

        m_Stale = false;
    }
}
