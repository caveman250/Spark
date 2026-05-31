#pragma once

#include "spark.h"
#include "engine/asset/shader/ast/Types.h"
#include "engine/math/math.h"
#include "engine/asset/texture/Texture.h"
#include "engine/asset/AssetReference.h"

namespace se::render
{
    class MaterialInstance;
}

namespace se::render
{
    struct UniformValueBase : reflect::ObjectBase
    {
        SPARK_CLASS(Abstract)

        ~UniformValueBase() override = default;
        virtual const void* GetValue() const = 0;
        virtual size_t GetValueCount() const = 0;
        virtual asset::shader::ast::AstType GetShaderType() const = 0;
        virtual void SetValue(const void* val, int count, bool internal) = 0;
        virtual UniformValueBase* Copy() = 0;

        bool internal = false;
    };

    template <typename T>
    struct UniformValue : UniformValueBase
    {
        SPARK_CLASS_TEMPLATED()
        const void* GetValue() const override { return static_cast<const void*>(value.data()); }
        size_t GetValueCount() const override { return value.size(); }
        asset::shader::ast::AstType GetShaderType() const override;
        void SetValue(const void* val, int count, bool internal) override;
        UniformValueBase* Copy() override;

        SPARK_MEMBER(Serialized)
        std::vector<T> value = {};
    };

    template<typename T>
    asset::shader::ast::AstType GetASTType() { return asset::shader::ast::AstType::Invalid; }

    SPARK_INSTANTIATE_TEMPLATE(UniformValue, int)
    template<>
    inline asset::shader::ast::AstType GetASTType<int>() { return asset::shader::ast::AstType::Int; }

    SPARK_INSTANTIATE_TEMPLATE(UniformValue, float)
    template<>
    inline asset::shader::ast::AstType GetASTType<float>() { return asset::shader::ast::AstType::Float; }

    SPARK_INSTANTIATE_TEMPLATE(UniformValue, math::Vec2)
    template<>
    inline asset::shader::ast::AstType GetASTType<math::Vec2>() { return asset::shader::ast::AstType::Vec2; }

    SPARK_INSTANTIATE_TEMPLATE(UniformValue, math::Vec3)
    template<>
    inline asset::shader::ast::AstType GetASTType<math::Vec3>() { return asset::shader::ast::AstType::Vec3; }

    SPARK_INSTANTIATE_TEMPLATE(UniformValue, math::Vec4)
    template<>
    inline asset::shader::ast::AstType GetASTType<math::Vec4>() { return asset::shader::ast::AstType::Vec4; }

    SPARK_INSTANTIATE_TEMPLATE(UniformValue, math::Mat3)
    template<>
    inline asset::shader::ast::AstType GetASTType<math::Mat3>() { return asset::shader::ast::AstType::Mat3; }

    SPARK_INSTANTIATE_TEMPLATE(UniformValue, math::Mat4)
    template<>
    inline asset::shader::ast::AstType GetASTType<math::Mat4>() { return asset::shader::ast::AstType::Mat4; }

    SPARK_INSTANTIATE_TEMPLATE(UniformValue, asset::AssetReference<asset::Texture>)
    template<>
    inline asset::shader::ast::AstType GetASTType<asset::AssetReference<asset::Texture>>() { return asset::shader::ast::AstType::Sampler2DReference; }

    SPARK_INSTANTIATE_TEMPLATE(UniformValue, std::shared_ptr<asset::Texture>)
    template<>
    inline asset::shader::ast::AstType GetASTType<std::shared_ptr<asset::Texture>>() { return asset::shader::ast::AstType::Sampler2D; }

    class UniformStorage : public reflect::ObjectBase
    {
        SPARK_CLASS()
    public:
        ~UniformStorage() override;
        UniformStorage() = default;
        UniformStorage(const UniformStorage& other);
        template <typename T>
        void SetValue(const std::string& name, int count, const T* value, bool internal);
        void SetValueDefault(const std::string& name, asset::shader::ast::AstType type);
        template<typename T>
        const T* GetValue(const std::string& name);
        bool HasValue(const std::string& name);
        void Apply(MaterialInstance* material);
        bool IsStale() const { return m_Stale; }
        void ApplyTo(UniformStorage& other) const;
        void Reset();
    private:
        SPARK_MEMBER(Serialized)
        std::map<std::string, UniformValueBase*> m_Storage;

        SPARK_MEMBER()
        bool m_Stale = false;
    };

    template<typename T>
    asset::shader::ast::AstType UniformValue<T>::GetShaderType() const
    {
        return GetASTType<T>();
    }

    template<typename T>
    void UniformValue<T>::SetValue(const void *val, int count, bool _internal)
    {
        value.clear();
        const T* typedVal = static_cast<const T*>(val);
        value.reserve(count);
        for (int i = 0; i < count; ++i) 
        {
            value.push_back(typedVal[i]);
        }
        internal = _internal;
    }

    template<typename T>
    UniformValueBase* UniformValue<T>::Copy()
    {
        return new UniformValue(*this);
    }

    template <typename T>
    void UniformStorage::SetValue(const std::string& name, int count, const T* value, bool internal)
    {
        if (m_Storage.contains(name))
        {
            auto* oldVal = m_Storage.at(name);
            if (!oldVal ||
                memcmp(static_cast<const void*>(value), oldVal->GetValue(), sizeof(T) * count) != 0 ||
                static_cast<int>(oldVal->GetValueCount()) != count)
            {
                m_Storage.at(name)->SetValue(value, count, internal);
                m_Stale = true;
            }
        }
        else
        {
            auto* uniformVal = new UniformValue<T>();
            uniformVal->SetValue(value, count, internal);
            m_Storage.insert(std::make_pair(name, uniformVal));
            m_Stale = true;
        }
    }

    template<typename T>
    const T* UniformStorage::GetValue(const std::string& name)
    {
        if (m_Storage.contains(name))
        {
            return static_cast<const T*>(m_Storage.at(name)->GetValue());
        }

        return nullptr;
    }
}

#include "se_render_UniformValue.generated.h"