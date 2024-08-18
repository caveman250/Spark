#pragma once

#include "spark.h"
#include "engine/asset/shader/ast/Types.h"

namespace se::render
{
    class Material;
}

namespace se::render
{
    struct UniformValueBase
    {
        virtual ~UniformValueBase() = default;
        virtual const void* GetValue() const = 0;
        virtual void SetValue(const void* val, int count) = 0;

        asset::shader::ast::AstType::Type type;
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
        void SetValue(const std::string& name, asset::shader::ast::AstType::Type type, int count, const T* value);
        void Apply(Material* material);
        bool IsStale() const { return m_Stale; }
    private:
        std::map<std::string, UniformValueBase*> m_Storage;
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
    void UniformStorage::SetValue(const std::string& name, asset::shader::ast::AstType::Type type, int count, const T* value)
    {
        if (m_Storage.contains(name))
        {
            m_Storage.at(name)->SetValue(value, count);
        }
        else
        {
            auto* uniformVal = new UniformValue<T>();
            uniformVal->SetValue(value, count);
            uniformVal->type = type;
            m_Storage.insert(std::make_pair(name, uniformVal));
        }

        m_Stale = true;
    }
}
