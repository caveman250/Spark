#include "MaterialInstanceEditor.h"

#include "engine/render/Material.h"
#include "engine/render/MaterialInstance.h"

namespace se::editor::ui::properties
{
    DEFINE_PROPERTY_EDITOR(render::MaterialInstance, MaterialInstanceEditor, MaterialInstance);

    void MaterialInstanceEditor::Update()
    {
        ClassEditor::Update();

        auto* materialInstance = static_cast<render::MaterialInstance*>(m_Value);
        render::UniformStorage& uniforms = materialInstance->GetUniforms();
        const auto& material = materialInstance->GetMaterial();
        if (!material)
        {
            if (m_CachedUniformsCount > 0)
            {
                uniforms.Reset();
                m_CachedUniformsCount = 0;
            }
            return;
        }
        const auto& vertShaders = material->GetVertShaders();
        const auto& fragShaders = material->GetFragShaders();
        int numUniforms = 0;
        for (const auto& shader : vertShaders)
        {
            if (shader.IsSet())
            {
                numUniforms += static_cast<int>(shader.GetAsset()->GetUniformVariables().size());
            }
        }
        for (const auto& shader : fragShaders)
        {
            if (shader.IsSet())
            {
                numUniforms += static_cast<int>(shader.GetAsset()->GetUniformVariables().size());
            }
        }

        if (m_CachedUniformsCount != numUniforms)
        {
            for (const auto& shader : vertShaders)
            {
                if (shader.IsSet())
                {
                    const auto& shaderAsset = shader.GetAsset();
                    for (const auto& uniform : shaderAsset->GetUniformVariables())
                    {
                        if (!uniform.second.internal && !uniforms.HasValue(uniform.first))
                        {
                            uniforms.SetValueDefault(uniform.first, uniform.second.var.type);
                        }
                    }
                }
            }
            for (const auto& shader : fragShaders)
            {
                if (shader.IsSet())
                {
                    const auto& shaderAsset = shader.GetAsset();
                    for (const auto& uniform : shaderAsset->GetUniformVariables())
                    {
                        if (!uniform.second.internal && !uniforms.HasValue(uniform.first))
                        {
                            uniforms.SetValueDefault(uniform.first, uniform.second.var.type);
                        }
                    }
                }
            }
            m_CachedUniformsCount = numUniforms;
        }
    }
}
