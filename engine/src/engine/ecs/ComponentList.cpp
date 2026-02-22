#include "ComponentList.h"
#include "engine/ui/components/TextComponent.h"

namespace se::ecs
{
    ComponentList::ComponentList(reflect::Type* type)
        : m_ElementType(type)
    {
        m_Data = static_cast<uint8_t*>(std::malloc(1000 * type->GetTypeSize())); // TODO
    }

    uint8_t* ComponentList::GetComponent(size_t index) const
    {
        return m_Data + index * m_ElementType->GetTypeSize();
    }

    void ComponentList::AddComponent(uint8_t* data)
    {
        uint8_t* comp = GetComponent(m_Count);
        m_ElementType->inplace_move_constructor(comp, data);
        m_Count++;
    }

    void ComponentList::RemoveComponent(size_t index)
    {
        if (SPARK_VERIFY(index < m_Count))
        {
            const uint64_t lastIndex = m_Count - 1;
            for (; index < lastIndex; ++index)
            {
                const uint64_t oldIndex = index + 1;
                uint8_t* oldData = GetComponent(oldIndex);

                uint8_t* dest = GetComponent(index);
                m_ElementType->inplace_move_constructor(dest, oldData);
            }
            m_Count--;
        }
    }
}