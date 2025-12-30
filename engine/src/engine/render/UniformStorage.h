#pragma once

#include "spark.h"
#include "engine/asset/shader/ast/Types.h"
#include "engine/reflect/Reflect.h"

namespace se::render
{
    class MaterialInstance;
}

namespace se::render
{
    struct UniformValueBase : reflect::ObjectBase
    {
        SPARK_CLASS(Abstract)

        virtual ~UniformValueBase() = default;
        virtual const void* GetValue() const = 0;
        virtual void SetValue(const void* val, int count) = 0;
        virtual UniformValueBase* Copy() = 0;

        SPARK_MEMBER(Serialized)
        asset::shader::ast::AstType type;

        SPARK_MEMBER(Serialized)
        int valueCount;
    };

    template <typename T>
    struct UniformValue : UniformValueBase
    {
        SPARK_CLASS_TEMPLATED()
        const void* GetValue() const override { return static_cast<const void*>(value.data()); }
        void SetValue(const void* val, int count) override;
        UniformValueBase* Copy() override;

        SPARK_MEMBER(Serialized)
        std::vector<T> value = {};
    };

    SPARK_INSTANTIATE_TEMPLATE(UniformValue, int)
    SPARK_INSTANTIATE_TEMPLATE(UniformValue, float)
    SPARK_INSTANTIATE_TEMPLATE(UniformValue, math::Vec2)
    SPARK_INSTANTIATE_TEMPLATE(UniformValue, math::Vec3)
    SPARK_INSTANTIATE_TEMPLATE(UniformValue, math::Mat3)
    SPARK_INSTANTIATE_TEMPLATE(UniformValue, math::Mat4)
    SPARK_INSTANTIATE_TEMPLATE(UniformValue, asset::AssetReference<asset::Texture>)
    SPARK_INSTANTIATE_TEMPLATE(UniformValue, std::shared_ptr<asset::Texture>)

    class UniformStorage : public reflect::ObjectBase
    {
        SPARK_CLASS()
    public:
        ~UniformStorage();
        template <typename T>
        void SetValue(const std::string& name, asset::shader::ast::AstType type, int count, const T* value);
        template<typename T>
        const T* GetValue(const std::string& name);
        void Apply(MaterialInstance* material);
        bool IsStale() const { return m_Stale; }
        void ApplyTo(UniformStorage& other) const;
    private:
        SPARK_MEMBER(Serialized)
        std::map<std::string, UniformValueBase*> m_Storage;

        SPARK_MEMBER()
        bool m_Stale = false;
    };

    template<typename T>
    void UniformValue<T>::SetValue(const void *val, int count)
    {
        value.clear();
        const T* typedVal = static_cast<const T*>(val);
        value.reserve(count);
        for (int i = 0; i < count; ++i) 
        {
            value.push_back(typedVal[i]);
            valueCount = count;
        }
    }

    template<typename T>
    UniformValueBase* UniformValue<T>::Copy()
    {
        return new UniformValue(*this);
    }

    template <typename T>
    void UniformStorage::SetValue(const std::string& name, asset::shader::ast::AstType type, int count, const T* value)
    {
        if (m_Storage.contains(name))
        {
            const void* oldVal = m_Storage.at(name)->GetValue();
            if (memcmp(static_cast<const void*>(value), oldVal, sizeof(T) * count) != 0)
            {
                m_Storage.at(name)->SetValue(value, count);
                m_Stale = true;
            }
        }
        else
        {
            auto* uniformVal = new UniformValue<T>();
            uniformVal->SetValue(value, count);
            uniformVal->type = type;
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