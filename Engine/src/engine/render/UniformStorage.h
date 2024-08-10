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
        virtual void SetValue(const void* val) = 0;

        asset::shader::ast::AstType::Type type;
        int count;
    };

    template <typename T>
    struct UniformValue : UniformValueBase
    {
        const void* GetValue() const override { return static_cast<const void*>(&value); }
        void SetValue(const void* val) override { value = *(T*)val; }

        T value;
    };

    class UniformStorage
    {
    public:
        ~UniformStorage();

        template <typename T>
        void SetValue(const std::string& name, asset::shader::ast::AstType::Type type, int count, const T* value);
        void Apply(Material* material);
    private:
        std::map<std::string, UniformValueBase*> m_Storage;
    };

    template <typename T>
    void UniformStorage::SetValue(const std::string& name, asset::shader::ast::AstType::Type type, int count, const T* value)
    {
        if (m_Storage.contains(name))
        {
            m_Storage.at(name)->SetValue(value);
        }
        else
        {
            auto* uniformVal = new UniformValue<T>();
            uniformVal->value = *value;
            uniformVal->count = count;
            uniformVal->type = type;
            m_Storage.insert(std::make_pair(name, uniformVal));
        }
    }
}
