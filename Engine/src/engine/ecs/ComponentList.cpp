#include "ComponentList.h"

namespace se::ecs
{
    ComponentList::ComponentList(size_t elemSize)
        : m_ElemSize(elemSize)
    {
        m_Data = static_cast<uint8_t*>(std::malloc(1000 * elemSize)); // TODO
    }

    uint8_t* ComponentList::GetComponent(size_t index) const
    {
        return m_Data + index * m_ElemSize;
    }

    void ComponentList::AddComponent(uint8_t* data)
    {
        uint8_t* comp = GetComponent(m_Count);
        std::memcpy(comp, data, m_ElemSize);
        m_Count++;
    }

    void ComponentList::RemoveComponent(size_t index)
    {
        if SPARK_VERIFY(index < m_Count)
        {
            const uint64_t lastIndex = m_Count - 1;
            if (index < lastIndex)
            {
                const uint64_t fragIndex = index + 1;
                uint8_t* fragStart = GetComponent(fragIndex);
                uint64_t fragSize = GetComponent(m_Count) - fragStart;

                uint8_t* dest = GetComponent(index);

                std::memcpy(dest, fragStart, fragSize);
            }
            m_Count--;
        }
    }
}