#include "Material.h"

namespace se::render
{
    Material::Material(const std::string &vertPath, const std::string &fragPath)
        : m_VertShaderPath(vertPath)
        , m_FragShaderPath(fragPath)
    {
    }
}


