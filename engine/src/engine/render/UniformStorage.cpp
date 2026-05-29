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

    void render::UniformStorage::SetValueDefault(const std::string& name,
        asset::shader::ast::AstType type)
    {
        switch (type)
        {
            case asset::shader::ast::AstType::Int:
            {
                int i = 0;
                SetValue(name, 1, &i, false);
                break;
            }
            case asset::shader::ast::AstType::Float:
            {
                float val = 0;
                SetValue(name, 1, &val, false);
                break;
            }
            case asset::shader::ast::AstType::Vec2:
            {
                math::Vec2 val = {};
                SetValue(name, 1, &val, false);
                break;
            }
            case asset::shader::ast::AstType::Vec3:
            {
                math::Vec3 val = {};
                SetValue(name, 1, &val, false);
                break;
            }
            case asset::shader::ast::AstType::Vec4:
            {
                math::Vec4 val = {};
                SetValue(name, 1, &val, false);
                break;
            }
            case asset::shader::ast::AstType::Mat3:
            {
                math::Mat3 val = {};
                SetValue(name, 1, &val, false);
                break;
            }
            case asset::shader::ast::AstType::Mat4:
            {
                math::Mat4 val = {};
                SetValue(name, 1, &val, false);
                break;
            }
            case asset::shader::ast::AstType::Bool:
            case asset::shader::ast::AstType::None:
            case asset::shader::ast::AstType::Void:
            case asset::shader::ast::AstType::Sampler2D:
            case asset::shader::ast::AstType::Sampler2DReference:
            case asset::shader::ast::AstType::Invalid:
                SPARK_ASSERT(false);
                break;
        }
    }

    bool render::UniformStorage::HasValue(const std::string& name)
    {
        return m_Storage.contains(name);
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

    void render::UniformStorage::Reset()
    {
        for (const auto& kvp : m_Storage)
        {
            delete kvp.second;
        }
        m_Storage.clear();
    }
}
