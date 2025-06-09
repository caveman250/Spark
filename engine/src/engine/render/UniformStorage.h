#pragma once

#include "spark.h"
#include "engine/asset/shader/ast/Types.h"

namespace se::render
{
    class MaterialInstance;
}

namespace se::render
{
    struct UniformValueBase
    {
        virtual ~UniformValueBase() = default;
        virtual const void* GetValue() const = 0;
        virtual void SetValue(const void* val, int count) = 0;

        asset::shader::ast::AstType type;
        int valueCount;
    };

    template <typename T>
    struct UniformValue : UniformValueBase
    {
        const void* GetValue() const override { return static_cast<const void*>(value.data()); }
        void SetValue(const void* val, int count) override;

        std::vector<T> value;
    };

    class UniformStorage
    {
    public:
        ~UniformStorage();

        template <typename T>
        void SetValue(const String& name, asset::shader::ast::AstType type, int count, const T* value);
        template<typename T>
        const T* GetValue(const String& name);
        void Apply(MaterialInstance* material);
        bool IsStale() const { return m_Stale; }
    private:
        std::map<String, UniformValueBase*> m_Storage;
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

    template <typename T>
    void UniformStorage::SetValue(const String& name, asset::shader::ast::AstType type, int count, const T* value)
    {
        if (m_Storage.contains(name))
        {
            auto* oldVal = static_cast<const T*>(m_Storage.at(name)->GetValue());
            if (memcmp(value, oldVal, sizeof(T) * count) != 0)
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
    const T* UniformStorage::GetValue(const String& name)
    {
        if (m_Storage.contains(name))
        {
            return static_cast<const T*>(m_Storage.at(name)->GetValue());
        }

        return nullptr;
    }
}
