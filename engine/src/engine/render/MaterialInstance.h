#pragma once
#include "UniformStorage.h"
#include "engine/asset/shader/ast/Types.h"

namespace se::asset::shader::ast
{
    enum class Type;
}

namespace se::render
{
    class VertexBuffer;
    class Material;

    class MaterialInstance : public reflect::ObjectBase
    {
        SPARK_CLASS(Abstract)
    public:
        static std::shared_ptr<MaterialInstance> CreateMaterialInstance(const std::shared_ptr<Material>& material);

        virtual ~MaterialInstance() = default;

        virtual void Bind(const VertexBuffer& vb);
        virtual void CreatePlatformResources();
        virtual void DestroyPlatformResources();

        template <typename T>
        void SetUniform(const std::string& name, asset::shader::ast::AstType type, int count, const T* value);

        template <typename T>
        const T* GetUniform(const std::string& name);
    protected:
        MaterialInstance(const std::shared_ptr<Material>& material);
        virtual void SetUniformInternal(const std::string& name, asset::shader::ast::AstType type, int count, const void* value) = 0;

        const std::shared_ptr<Material> m_Material = nullptr;

        SPARK_MEMBER(Serialized)
        UniformStorage m_UniformStorage = {};

        SPARK_MEMBER()
        bool m_PlatformResourcesCreated = false;

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
