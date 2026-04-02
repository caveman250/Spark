#pragma once
#include "UniformStorage.h"
#include "engine/asset/shader/ast/Types.h"
#include "Material.h"

namespace se::asset::shader::ast
{
    enum class Type;
}

namespace se::render
{
    class MaterialInstancePlatformResources;
    class VertexBuffer;

    class MaterialInstance : public asset::Asset
    {
        SPARK_CLASS()
    public:
        MaterialInstance() = default;
        ~MaterialInstance() override;
        explicit MaterialInstance(const asset::AssetReference<Material>& material);
        explicit MaterialInstance(const std::shared_ptr<Material>& material);

        static std::shared_ptr<MaterialInstancePlatformResources> CreateMaterialInstancePlatformResources(const std::shared_ptr<Material>& material);

        virtual void Bind(const VertexBuffer& vb);

        template <typename T>
        void SetUniform(const std::string& name, asset::shader::ast::AstType type, int count, const T* value);

        template <typename T>
        const T* GetUniform(const std::string& name);

        const std::shared_ptr<Material>& GetMaterial();
        const std::shared_ptr<MaterialInstancePlatformResources>& GetPlatformResources() const { return m_PlatformResources; }
    protected:
        SPARK_MEMBER(Serialized)
        asset::AssetReference<Material> m_Material = {};

        std::shared_ptr<Material> m_MaterialHandle = {};

        SPARK_MEMBER(Serialized)
        UniformStorage m_UniformStorage = {};

        std::shared_ptr<MaterialInstancePlatformResources> m_PlatformResources = nullptr;

        friend class UniformStorage;
    };

    template <typename T>
    void MaterialInstance::SetUniform(const std::string& name, asset::shader::ast::AstType type, int count, const T* value)
    {
        m_UniformStorage.SetValue(name, type, count, value);
    }

    template<typename T>
    const T* MaterialInstance::GetUniform(const std::string& name)
    {
        return m_UniformStorage.GetValue<T>(name);
    }
}
