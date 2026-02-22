#pragma once

#include "engine/reflect/Reflect_fwd.h"

namespace se::ecs
{
    class ComponentList
    {
    public:
        explicit ComponentList(reflect::Type* type);

        uint8_t* GetComponent(size_t index) const;
        void AddComponent(uint8_t* data);
        void RemoveComponent(size_t index);

        uint8_t* Data() const { return m_Data; }
        size_t Count() const { return m_Count; }

    private:
        reflect::Type* m_ElementType = nullptr;
        size_t m_Count = 0;
        uint8_t* m_Data = nullptr;

        friend class World;
    };
}