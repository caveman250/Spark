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
            // ReSharper disable once CppDFAUnreachableCode
            for (ObjectRecord* objRecord = m_ObjectRecordsBegin; objRecord != nullptr; objRecord = objRecord->next)
            {
                objRecord->Release(objRecord->objects_begin);
            }
        }
    }
}