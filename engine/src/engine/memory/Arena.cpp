#include "Arena.h"

namespace se::memory
{
    Arena::Arena(size_t capacity)
    {
        m_Capacity = capacity;
        m_Arena = static_cast<char*>(malloc(m_Capacity));
    }

    Arena::~Arena()
    {
        ResetAndReleaseMemory();
    }

    void* Arena::Alloc(reflect::Type* type)
    {
        if (!SPARK_VERIFY(m_Current + type->size < m_Capacity))
        {
            return nullptr;
        }

        char* item = m_Arena + m_Current;
        m_Current += type->size;

        char* recordMemory = m_Arena + m_Current;
        ObjectRecordReflected* record = new (recordMemory)ObjectRecordReflected();
        record->type = type;
        m_Current += sizeof(ObjectRecordReflected);
        record->next = m_ObjectRecordsBegin;
        record->objects_begin = item;
        record->objects_count = 1;
        m_ObjectRecordsBegin = record;

        return type->inplace_constructor(item);
    }

    void Arena::Reset()
    {
        ReleaseObjects();
        m_Current = 0;
        m_ObjectRecordsBegin = nullptr;
    }

    void Arena::ResetAndReleaseMemory()
    {
        ReleaseObjects();
        free(m_Arena);
        m_Current = 0;
        m_ObjectRecordsBegin = nullptr;
    }

    void Arena::ReleaseObjects() const
    {
        if (m_ObjectRecordsBegin)
        {
            for (ObjectRecord* objRecord = m_ObjectRecordsBegin; objRecord != nullptr; objRecord = objRecord->next)
            {
                objRecord->Release(objRecord->objects_begin);
            }
        }
    }
}